#pragma once

#include "vox/world/chunk.hpp"
#include <glm/gtx/hash.hpp>

struct BlockPosition {
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

	RaycastResult raycast(vec3 start, vec3 dir, f32 max_distance) const;

	void set_block(BlockPosition position, BlockID value);
	BlockID get_block(BlockPosition position) const;

	static BlockPosition get_block_position(vec3 global_position);

	Chunk *get_chunk(ChunkPosition position) const;

private:
	mutable std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
