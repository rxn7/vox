#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/common/world/world_constants.hpp"
#include "vox/common/world/block_id.hpp"
#include "vox/common/world/chunk_position.hpp"
#include <bitset>

class World;
class SubChunk;

class Chunk {
public:
	Chunk(World &world, ChunkPosition position);

	BlockID get_block(LocalBlockPosition pos) const;

	void set_block(LocalBlockPosition pos, BlockID value);

	bool is_block_transparent(i8 x, i16 y, i8 z) const;

	void set_all_non_empty_subchunks_dirty();

	inline bool is_dirty(u32 subchunk_idx) const {
		return m_dirty_subchunks_bitmap[subchunk_idx];
	}

	inline void set_dirty(u32 subchunk_idx, bool value) {
		m_dirty_subchunks_bitmap[subchunk_idx] = value;
	}

	inline bool has_dirty_subchunks() const {
		return m_dirty_subchunks_bitmap.any();
	}

	inline ChunkPosition get_position() const { 
		return m_position; 
	}

	constexpr vec3 get_global_position() const { 
		return vec3(m_position.x, 0, m_position.y) * CHUNK_WIDTH; 
	}

	inline const std::array<std::unique_ptr<SubChunk>, SUBCHUNK_COUNT> &get_subchunks() const {
		return m_subchunks;
	}

	inline SubChunk *get_subchunk(u32 idx) const {
		if(idx >= SUBCHUNK_COUNT) {
			return nullptr;
		}
		return m_subchunks[idx].get();
	}

private:
	ChunkPosition m_position;
	World &m_world;

	std::array<std::unique_ptr<SubChunk>, SUBCHUNK_COUNT> m_subchunks;
	std::bitset<SUBCHUNK_COUNT> m_dirty_subchunks_bitmap;
};
