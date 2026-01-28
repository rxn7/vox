#include "vox/common/server/server_logic.hpp"

template<typename ...T>
void server_log(std::format_string<T...> fmt, T &&...values) {
	std::print("[SERVER] ");
	std::println(fmt, std::forward<T>(values)...);
}

ServerLogic::ServerLogic(std::shared_ptr<IServerDriver> p_network) : mp_network(p_network) {
	m_world.create_initial_chunks();

	p_network->m_player_connected.connect([&](i32 client_id) {
		for(i32 x = -4; x < 4; ++x) {
			for(i32 z = -4; z < 4; ++z) {
				send_chunk_to_client(client_id, {x, z});
			}
		}
	});
}

void ServerLogic::run(std::stop_token stop_token) {
	server_log("Started");

	m_tick_loop.start();
	while(!stop_token.stop_requested()) {
		m_tick_loop.accumulate();
		while(m_tick_loop.consume_tick()) {
			tick();
		}

		C2S_Packet packet;
		i32 sender_id;
		while(mp_network->poll_packet(packet, sender_id)) {
			handle_packet(packet, sender_id);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void ServerLogic::tick() {
}

void ServerLogic::send_chunk_to_client(i32 client_id, ChunkPosition position) {
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

		std::memcpy(
			ferry_data->data(),
			subchunk->get_blocks().data(),
			SUBCHUNK_VOLUME
		);

		packet.data[i] = ferry_data;
	}

	mp_network->send_packet_to_client(client_id, std::move(packet));
}

void ServerLogic::handle_packet(const C2S_Packet &packet, i32 sender_id) {
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
		},
	}, packet);
}
