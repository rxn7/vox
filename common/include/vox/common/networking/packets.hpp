#pragma once

#include "vox/common/networking/packet_header.hpp"

constexpr u32 CHAT_MESSAGE_LENGTH = 256;
struct ChatMessagePacket : PacketHeader {
	constexpr ChatMessagePacket(std::string_view msg) : PacketHeader(PacketType::CHAT_MESSAGE) {
		memset(message, 0,CHAT_MESSAGE_LENGTH);
		std::strncpy(message, msg.data(), CHAT_MESSAGE_LENGTH);
	}

	char message[CHAT_MESSAGE_LENGTH];
};
