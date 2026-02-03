#pragma once

// NOTE: C2S = Client to Server

struct C2S_ChatMessagePacket {
	std::string message;
};

struct C2S_PlayerUpdatePacket {
	vec3 position;
	f32 pitch, yaw;
};

using C2S_Packet = std::variant<
	C2S_ChatMessagePacket,
	C2S_PlayerUpdatePacket
>;
