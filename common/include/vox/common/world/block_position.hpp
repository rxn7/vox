#pragma once

#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/world_constants.hpp"

// block position in a chunk
struct LocalBlockPosition {
	u8 x = 0;
	u16 y = 0;
	u8 z = 0;

	constexpr LocalBlockPosition() {}
	constexpr LocalBlockPosition(u8 x, u16 y, u8 z) : x(x), y(y), z(z) {}

	inline bool operator==(const LocalBlockPosition &other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	inline operator vec3() const { return vec3(x, y, z); }
	inline operator ivec3() const { return ivec3(x, y, z); }
};

// block position in the world
struct BlockPosition {
	constexpr BlockPosition() {}
	constexpr BlockPosition(ChunkPosition chunk_position, LocalBlockPosition local_position) : chunk_position(chunk_position), local_position(local_position) {}
	explicit BlockPosition(vec3 global_position);

	inline bool is_valid() const {
		return local_position.y < CHUNK_HEIGHT && local_position.x < CHUNK_WIDTH && local_position.z < CHUNK_WIDTH;
	}

	inline bool operator==(const BlockPosition &other) const {
		return chunk_position == other.chunk_position && local_position == other.local_position;
	}

	inline ivec3 get_global_position() const {
		return ivec3(chunk_position.x * SUBCHUNK_SIZE, 0, chunk_position.y * SUBCHUNK_SIZE) + ivec3(local_position);
	}

	ChunkPosition chunk_position;
	LocalBlockPosition local_position;
};
