#pragma once

#include "vox/common/helper/signal.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/world/block_position.hpp"

#include <glm/gtx/hash.hpp>

struct RaycastResult {
	bool m_did_hit = false;
	BlockPosition m_hit_block_position; // used for breaking blocks
	ivec3 m_previous_grid_position; // used for placing blocks
	f32 m_distance = 0.0f;
};

class World {
public:
	World();
	~World();
	
	void create_initial_chunks();

	void create_chunk(ChunkPosition position);
	void remove_chunk(ChunkPosition position);
	void mark_all_chunks_dirty();

	RaycastResult raycast(vec3 start, vec3 dir, f32 max_distance) const;

	void set_block(BlockPosition position, BlockID value);
	BlockID get_block(BlockPosition position) const;
	Chunk *get_chunk(ChunkPosition position) const;
	
	std::unordered_map<ChunkPosition, Chunk> &get_chunks() const { 
		return m_chunks; 
	}
	 
public:
	Signal<World, Chunk&> m_chunk_added_signal;
	Signal<World, Chunk&> m_chunk_removed_signal;

private:
	mutable std::unordered_map<ChunkPosition, Chunk> m_chunks; 
};
