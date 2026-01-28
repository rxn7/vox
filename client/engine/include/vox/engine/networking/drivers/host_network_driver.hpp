#pragma once

#include "vox/common/helper/thread_safe_queue.hpp"
#include "vox/common/networking/drivers/i_client_driver.hpp"
#include "vox/common/networking/drivers/i_server_driver.hpp"

class HostNetworkDriver : public IClientDriver, public IServerDriver {
public:
	// CLIENT
	void SendPacketToServer(C2S_Packet packet) override;
	bool PollPacket(S2C_Packet &out_packet) override;

	// SERVER
	void SendPacketToClient(i32 client_id, S2C_Packet packet) override;
	bool PollPacket(C2S_Packet &out_packet, i32 &sender_id) override;

private:
	ThreadSafeQueue<C2S_Packet> m_packets_for_server_queue;
	ThreadSafeQueue<S2C_Packet> m_packets_for_client_queue;
};
