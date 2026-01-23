#pragma once

#include "vox/common/world/world_constants.hpp"
#include "vox/common/world/block_id.hpp"
#include "vox/common/world/chunk_position.hpp"

constexpr u32 TOTAL_BLOCKS = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;

class World;

class Chunk {
public:
	Chunk(World &world, ChunkPosition position);

	bool is_block_transparent(i8 x, i8 y, i8 z) const;

	constexpr vec3 get_global_position() const { 
		return vec3(m_position) * CHUNK_WIDTH; 
	}

	constexpr u16 get_block_idx(u8vec3 v) const {
		return v.y * CHUNK_WIDTH * CHUNK_WIDTH + v.z * CHUNK_WIDTH + v.x;
	}

	inline BlockID get_block(u8vec3 v) const { 
		return m_blocks[get_block_idx(v)]; 
	}

	inline void set_block(u8vec3 pos, BlockID value) {
		m_blocks[pos.y * CHUNK_WIDTH * CHUNK_WIDTH + pos.z * CHUNK_WIDTH + pos.x] = value;
	}

	inline bool is_dirty() const {
		return m_is_dirty;
	}

	inline void set_dirty(bool value) {
		m_is_dirty = value;
	}

	inline ChunkPosition get_position() const { 
		return m_position; 
	}

private:
	World &m_world;
	std::array<BlockID, TOTAL_BLOCKS> m_blocks;

	ChunkPosition m_position;
	bool m_is_dirty;
};
