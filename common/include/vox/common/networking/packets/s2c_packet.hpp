#pragma once

#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/subchunk.hpp"

// NOTE: S2C = Server to Client

struct S2C_ChatMessagePacket {
	i32 sender_id;
	std::string message;
};

struct S2C_PlayerUpdatePacket {
	i32 player_id;
	vec3 position;
	float pitch, yaw;
};

struct S2C_ChunkUpdatePacket {
	ChunkPosition position;
	std::vector<std::shared_ptr<SubChunkData>> data;
};

typedef std::variant<
	S2C_ChatMessagePacket,
	S2C_PlayerUpdatePacket,
	S2C_ChunkUpdatePacket
> S2C_Packet;
