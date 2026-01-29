#include "vox/client/networking/drivers/host_network_driver.hpp"

constexpr i32 HOST_CLIENT_ID = 0;

void HostNetworkDriver::init() {
	m_player_connected.emit(HOST_CLIENT_ID);
}

// CLIENT
void HostNetworkDriver::send_packet_to_server(C2S_Packet packet) {
	m_packets_for_server_queue.push(std::move(packet));
}

bool HostNetworkDriver::poll_packet(S2C_Packet &out_packet) {
	return m_packets_for_client_queue.try_pop(out_packet);
}

// SERVER
void HostNetworkDriver::send_packet_to_client(i32 sender_id, S2C_Packet packet) {
	// handle it locally if host sent the packet to itself
	if(sender_id == HOST_CLIENT_ID) {
		m_packets_for_client_queue.push(std::move(packet));
		return;
	}

	// TODO: send the packet remotely if server is enabled
}

void HostNetworkDriver::broadcast_packet(S2C_Packet packet, i32 except_id) {
	if(except_id != HOST_CLIENT_ID) {
		m_packets_for_client_queue.push(std::move(packet));
		return;
	}
	
	// TODO: Send for each connected client
}

bool HostNetworkDriver::poll_packet(C2S_Packet &out_packet, i32 &sender_id) {
	// check for local packets first
	if(m_packets_for_server_queue.try_pop(out_packet)) {
		sender_id = HOST_CLIENT_ID;
		return true;
	}

	// TODO: poll remote packets if host has enabled the server
	
	return false;
}
