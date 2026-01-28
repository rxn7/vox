#pragma once

#include "vox/common/helper/signal.hpp"
#include "vox/common/networking/packets/s2c_packet.hpp"
#include "vox/common/networking/packets/c2s_packet.hpp"

class IServerDriver {
public:
	virtual ~IServerDriver() = default;

	virtual void send_packet_to_client(i32 client_id, S2C_Packet packet) = 0;
	virtual void broadcast_packet(S2C_Packet packet, i32 except_id = -1) = 0;
	virtual bool poll_packet(C2S_Packet &out_packet, i32 &sender_id) = 0;

public:
	Signal<i32> m_player_connected;
	Signal<i32> m_player_disconnected;
};
