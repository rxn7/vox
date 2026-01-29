#include "vox/common/server/server_logic.hpp"

template<typename ...T>
void server_log(std::format_string<T...> fmt, T &&...values) {
	std::print("[SERVER] ");
	std::println(fmt, std::forward<T>(values)...);
}

ServerLogic::ServerLogic(std::shared_ptr<IServerDriver> p_network) : mp_network(p_network) {
	m_world.create_initial_chunks();

	p_network->m_player_connected.connect([&](i32 client_id) {
		m_players.insert({client_id, PlayerServerEntity{}});

		for(i32 x = -4; x < 4; ++x) {
			for(i32 z = -4; z < 4; ++z) {
				send_chunk_to_client(client_id, {x, z});
			}
		}
	});

	p_network->m_player_disconnected.connect([&](i32 client_id) {
		m_players.erase(client_id);
	});
}

void ServerLogic::run(std::stop_token stop_token) {
#ifndef NDEBUG
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

	for(const auto &[player_id, player_entity] : m_players) {
		// TODO: this should be on the client
	}
}

void ServerLogic::send_chunk_to_client(i32 client_id, ChunkPosition position) {
	PROFILE_FUNC();

	Chunk *chunk = m_world.get_chunk(position);
	if(chunk == nullptr) {
		server_log("Chunk doesn't exist");

		chunk = m_world.create_chunk(position);
		m_world.generate_chunk(*chunk);
	}

	S2C_ChunkUpdatePacket packet;
	packet.position = position;
	packet.data.resize(SUBCHUNK_COUNT);

	for(i32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		SubChunk *subchunk = chunk->get_subchunk(i);

		if(!subchunk || subchunk->is_empty()) {
			packet.data[i] = nullptr;
			continue;
		}

		std::shared_ptr<SubChunkData> ferry_data = std::make_shared<SubChunkData>();

		const SubChunkData &blocks = subchunk->get_blocks();
		std::copy(blocks.begin(), blocks.end(), ferry_data->begin());

		packet.data[i] = ferry_data;
	}

	mp_network->send_packet_to_client(client_id, std::move(packet));
}

void ServerLogic::handle_packet(C2S_Packet packet, i32 sender_id) {
	PROFILE_FUNC();

	std::visit(Overloaded {
		[&](const C2S_ChatMessagePacket &p)  {
			// TODO: Handle commands
			server_log("[CHAT] {}: {}", sender_id, p.message);

			S2C_ChatMessagePacket broadcast_packet = {
				.sender_id = sender_id,
				.message = p.message
			};

			mp_network->broadcast_packet(std::move(broadcast_packet));
		},

		[&](const C2S_PlayerUpdatePacket &p)  {
			if(!m_players.contains(sender_id)) {
				server_log("Invalid sender id: {}", sender_id);
				return;
			}

			PlayerServerEntity &player_entity = m_players.at(sender_id);
			player_entity.update(p.position, p.pitch, p.yaw);

			const BlockPosition position = player_entity.get_block_position();

			// TODO: Temporary
			Chunk *chunk = m_world.get_chunk(position.chunk_position); 
			if(chunk == nullptr) {
				chunk = m_world.create_chunk(position.chunk_position);
				m_world.generate_chunk(*chunk);

				send_chunk_to_client(sender_id, position.chunk_position);
			}
		},
	}, packet);
}
