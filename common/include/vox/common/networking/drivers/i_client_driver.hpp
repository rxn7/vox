#pragma once

#include "vox/common/networking/packets/c2s_packet.hpp"
#include "vox/common/networking/packets/s2c_packet.hpp"

class IClientDriver {
public:
	virtual ~IClientDriver() = default;

	virtual void SendPacketToServer(C2S_Packet packet) = 0;
	virtual bool PollPacket(S2C_Packet &out_packet) = 0;
};
