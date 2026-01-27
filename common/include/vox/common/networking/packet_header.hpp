#pragma once

#include "vox/common/networking/packet_type.hpp"

struct PacketHeader {
	constexpr PacketHeader(PacketType type) : m_type(type) {}

	PacketType m_type;
};