#pragma once

#include "vox/common/networking/packets/s2c_packet.hpp"
#include "vox/common/networking/packets/c2s_packet.hpp"

class IServerDriver {
public:
	virtual ~IServerDriver() = default;

	virtual void SendPacketToClient(i32 client_id, S2C_Packet packet) = 0;
	virtual bool PollPacket(C2S_Packet &out_packet, i32 &sender_id) = 0;
};
