#pragma once

// NOTE: C2S = Client to Server

struct C2S_ChatMessagePacket {
	std::string message; // used vector to allocate on heap
};

struct C2S_PlayerUpdatePacket {
	vec3 position;
	f32 pitch, yaw;
};

typedef std::variant<
	C2S_ChatMessagePacket,
	C2S_PlayerUpdatePacket
> C2S_Packet;
