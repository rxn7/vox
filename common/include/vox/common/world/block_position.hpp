#pragma once

#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/world_constants.hpp"

struct BlockPosition {
	constexpr BlockPosition() {}
	constexpr BlockPosition(ChunkPosition chunk_position, u8vec3 local_position) : chunk_position(chunk_position), local_position(local_position) {}
	BlockPosition(vec3 global_position);

	inline bool operator==(const BlockPosition &other) const {
		return chunk_position == other.chunk_position && local_position == other.local_position;
	}

	inline ivec3 get_global_position() const {
		return vec3(chunk_position) * CHUNK_WIDTH + vec3(local_position);
	}

	ChunkPosition chunk_position;
	u8vec3 local_position;
};
