#pragma once

#include "vox/common/world/world.hpp"
#include "vox/common/system/tick_loop.hpp"
#include "vox/common/networking/drivers/i_server_driver.hpp"

class ServerLogic {
public:
	explicit ServerLogic(std::shared_ptr<IServerDriver> p_network);
	void run(std::stop_token stop_token);

private:
	void send_chunk_to_client(i32 client_id, ChunkPosition position);
	void tick();
	void handle_packet(const C2S_Packet &packet, i32 sender_id);

private:
	std::shared_ptr<IServerDriver> mp_network;
	World m_world;
	TickLoop m_tick_loop;
};
