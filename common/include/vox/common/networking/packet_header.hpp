#pragma once

#include "vox/common/networking/packet_type.hpp"

struct PacketHeader {
	constexpr PacketHeader(PacketType type) : type(type) {}

	PacketType type;
};
