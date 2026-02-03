#pragma once

#include "vox/common/server/player_server_entity.hpp"
#include "vox/common/server/server_world.hpp"
#include "vox/common/system/tick_loop.hpp"
#include "vox/common/networking/drivers/i_server_driver.hpp"

class ServerLogic {
public:
	explicit ServerLogic(std::shared_ptr<IServerDriver> p_network);
	void run(std::stop_token stop_token);

private:
	void tick();

	void handle_packet(C2S_Packet packet, i32 sender_id);
	void handle_chat_message_packet(C2S_ChatMessagePacket p, i32 sender_id);
	void handle_player_update_packet(C2S_PlayerUpdatePacket p, i32 sender_id);

	void update_client_chunks(i32 client_id, PlayerServerEntity &player);
	void send_chunk_update_to_client(i32 client_id, ChunkPosition position);
	void send_chunk_unload_to_client(i32 client_id, ChunkPosition position);

private:
	std::shared_ptr<IServerDriver> mp_network;
	ServerWorld m_world;
	std::unordered_map<i32, PlayerServerEntity> m_players;
	TickLoop m_tick_loop;
};
