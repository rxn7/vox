#include "vox/engine/networking/drivers/host_network_driver.hpp"

constexpr i32 HOST_CLIENT_ID = 0;

// CLIENT
void HostNetworkDriver::SendPacketToServer(C2S_Packet packet) {
	m_packets_for_server_queue.push(std::move(packet));
}

bool HostNetworkDriver::PollPacket(S2C_Packet &out_packet) {
	return m_packets_for_client_queue.try_pop(out_packet);
}

// SERVER
void HostNetworkDriver::SendPacketToClient(i32 sender_id, S2C_Packet packet) {
	// handle it locally if host sent the packet to itself
	if(sender_id == HOST_CLIENT_ID) {
		m_packets_for_client_queue.push(std::move(packet));
		return;
	}

	// TODO: send the packet remotely if server is enabled
}

bool HostNetworkDriver::PollPacket(C2S_Packet &out_packet, i32 &sender_id) {
	// check for local packets first
	if(m_packets_for_server_queue.try_pop(out_packet)) {
		sender_id = HOST_CLIENT_ID;
		return true;
	}

	// TODO: poll remote packets if host has enabled the server
	
	return false;
}
