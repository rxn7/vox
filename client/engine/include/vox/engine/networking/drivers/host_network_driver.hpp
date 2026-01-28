#pragma once

#include "vox/common/helper/thread_safe_queue.hpp"
#include "vox/common/networking/drivers/i_client_driver.hpp"
#include "vox/common/networking/drivers/i_server_driver.hpp"

class HostNetworkDriver : public IClientDriver, public IServerDriver {
public:
	void init();

	// CLIENT
	void send_packet_to_server(C2S_Packet packet) override;
	bool poll_packet(S2C_Packet &out_packet) override;

	// SERVER
	void send_packet_to_client(i32 client_id, S2C_Packet packet) override;
	void broadcast_packet(S2C_Packet packet, i32 except_id = -1) override;
	bool poll_packet(C2S_Packet &out_packet, i32 &sender_id) override;

private:
	ThreadSafeQueue<C2S_Packet> m_packets_for_server_queue;
	ThreadSafeQueue<S2C_Packet> m_packets_for_client_queue;
};
