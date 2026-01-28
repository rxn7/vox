#pragma once

#include "vox/common/networking/packets/c2s_packet.hpp"
#include "vox/common/networking/packets/s2c_packet.hpp"

class IClientDriver {
public:
	virtual ~IClientDriver() = default;

	virtual void send_packet_to_server(C2S_Packet packet) = 0;
	virtual bool poll_packet(S2C_Packet &out_packet) = 0;
};
