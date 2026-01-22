#pragma once

#include "vox/world/chunk.hpp"
#include <glm/gtx/hash.hpp>

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

struct RaycastResult {
	bool hit = false;
	BlockPosition hit_block_position; // used for breaking blocks
	ivec3 previous_grid_position; // used for placing blocks
	f32 distance = 0.0f;
};

class World {
public:
	World();
	~World();

	void render(WorldRenderer &renderer);
	void regenerate_all_chunks();

	RaycastResult raycast(vec3 start, vec3 dir, f32 max_distance) const;

	void set_block(BlockPosition position, BlockID value);
	BlockID get_block(BlockPosition position) const;

	Chunk *get_chunk(ChunkPosition position) const;

private:
	mutable std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
