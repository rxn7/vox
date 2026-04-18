#include "vox/common/server/server_logic.hpp"
#include "vox/common/world/chunk_offsets.hpp"

template<typename ...T>
void server_log(std::format_string<T...> fmt, T &&...values) {
	std::print("[SERVER] ");
	std::println(fmt, std::forward<T>(values)...);
}

ServerLogic::ServerLogic(std::shared_ptr<IServerDriver> p_network) : mp_network(p_network) {
	m_world.create_initial_chunks();

	p_network->m_player_connected.connect([&](i32 client_id) {
		m_players.insert({client_id, PlayerServerEntity{}});
	});

	p_network->m_player_disconnected.connect([&](i32 client_id) {
		m_players.erase(client_id);
	});
}

void ServerLogic::run(std::stop_token stop_token) {
#ifdef VOX_DEBUG
	Profiler::get_instance().register_this_thread("Server Logic");
#endif

	server_log("Started");

	m_tick_loop.start();
	while(!stop_token.stop_requested()) {
		PROFILE_SCOPE("Root Loop");

		m_tick_loop.accumulate();
		while(m_tick_loop.consume_tick()) {
			tick();
		}

		{
			PROFILE_SCOPE("Server packet polling")

			C2S_Packet packet;
			i32 sender_id;
			while(mp_network->poll_packet(packet, sender_id)) {
				handle_packet(std::move(packet), sender_id);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void ServerLogic::tick() {
	PROFILE_FUNC();

	for(auto &[id, player] : m_players) {
		update_client_chunks(id, player);
	}
}

void ServerLogic::handle_packet(C2S_Packet packet, i32 sender_id) {
	PROFILE_FUNC();

	if(!m_players.contains(sender_id)) {
		server_log("Invalid sender id: {}", sender_id);
		return;
	}

	std::visit(Overloaded {
		[&](C2S_ChatMessagePacket p) -> void {
			handle_chat_message_packet(std::move(p), sender_id);
		},
		[&](C2S_PlayerUpdatePacket p) -> void {
			handle_player_update_packet(std::move(p), sender_id);
		},
	}, packet);
}

void ServerLogic::handle_chat_message_packet(C2S_ChatMessagePacket p, i32 sender_id) {
	PROFILE_FUNC();

	// TODO: Handle commands
	server_log("[CHAT] {}: {}", sender_id, p.message);

	S2C_ChatMessagePacket broadcast_packet = {
		.sender_id = sender_id,
		.message = std::move(p.message)
	};

	mp_network->broadcast_packet(std::move(broadcast_packet));
}

void ServerLogic::handle_player_update_packet(C2S_PlayerUpdatePacket p, i32 sender_id) {
	PROFILE_FUNC();

	PlayerServerEntity &player_entity = m_players.at(sender_id);
	player_entity.update(p.position, p.pitch, p.yaw);
}

void ServerLogic::update_client_chunks(i32 client_id, PlayerServerEntity &player) {
	PROFILE_FUNC();

	const ChunkPosition center = player.get_block_position().chunk_position;

	// TODO: Dynamic render distance
	const u32 render_distance = MAX_SPIRAL_RADIUS / 4;
	const u32 render_distance_sqr = render_distance * render_distance;

	constexpr u32 MAX_CHUNKS_SENTS = 1;
	u32 chunks_sent = 0;

	for(const i16vec2 offset : CHUNK_OFFSETS) {
		const u32 dist_sqr = (offset.x * offset.x) + (offset.y * offset.y);

		if(dist_sqr > render_distance_sqr) {
			break;
		}

		const ChunkPosition target_position = center + offset;
		if(player.has_chunk_loaded(target_position)) {
			continue;
		}

		player.register_chunk_load(target_position);
		send_chunk_update_to_client(client_id, target_position);

		chunks_sent++;
		if(chunks_sent >= MAX_CHUNKS_SENTS) {
			break;
		}
	}

	u32 unload_radius_sqr = (render_distance + 2) * (render_distance + 2);
	std::vector<ChunkPosition> chunks_to_unload;

	for(const ChunkPosition pos : player.get_loaded_chunks()) {
		const i16vec2 delta = pos - center;
		const u32 dist_sqr = (delta.x * delta.x) + (delta.y * delta.y); 

		if(dist_sqr > unload_radius_sqr) {
			chunks_to_unload.emplace_back(pos);
		}
	}

	for(const ChunkPosition pos : chunks_to_unload) {
		player.register_chunk_unload(pos);
		send_chunk_unload_to_client(client_id, pos);
	}
}

void ServerLogic::send_chunk_update_to_client(i32 client_id, ChunkPosition position) {
	PROFILE_FUNC();

	std::shared_ptr<Chunk> p_chunk = m_world.get_chunk(position);
	if(p_chunk == nullptr) {
		p_chunk = m_world.create_chunk(position);
		m_world.generate_chunk(p_chunk); // TODO: async
	}

	S2C_ChunkUpdatePacket packet;
	packet.position = position;
	packet.subchunk_mask = 0;
	packet.data.reserve(SUBCHUNK_COUNT / 2);

	std::shared_lock<std::shared_mutex> lock(p_chunk->m_mutex);
	for(i32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		std::shared_ptr<SubChunk> p_subchunk = p_chunk->get_subchunk(i);

		if(!p_subchunk || p_subchunk->is_empty()) {
			continue;
		}

		packet.subchunk_mask |= (1 << i);
		packet.data.push_back(p_subchunk->get_blocks());
	}
	lock.unlock();

	mp_network->send_packet_to_client(client_id, std::move(packet));
}

void ServerLogic::send_chunk_unload_to_client(i32 client_id, ChunkPosition position) {
	PROFILE_FUNC();

	S2C_ChunkUnloadPacket packet;
	packet.position = std::move(position);

	mp_network->send_packet_to_client(client_id, std::move(packet));
}
