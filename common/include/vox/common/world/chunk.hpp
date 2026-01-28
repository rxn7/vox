#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/common/world/world_constants.hpp"
#include "vox/common/world/block_id.hpp"
#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/block_registry.hpp"
#include "vox/common/world/subchunk.hpp"

class IWorld;

class Chunk {
public:
	Chunk(IWorld &world, ChunkPosition position);

	BlockID get_block_local(LocalBlockPosition pos) const;

	BlockID get_block_relative(i8 x, i16 y, i8 z) const;

	void set_block_local(LocalBlockPosition pos, BlockID value);

	void set_block_relative(i8 x, i16 y, i8 z, BlockID value);

	void set_all_non_empty_subchunks_dirty();

	void remove_subchunk(u32 idx);

	inline bool is_block_transparent_relative(i8 x, i16 y, i8 z) const {
		return BlockRegistry::get(get_block_relative(x, y, z)).is_transparent();
	}

	inline bool is_dirty(u32 subchunk_idx) const {
		return m_dirty_subchunks_bitmap[subchunk_idx];
	}

	inline void set_dirty(u32 subchunk_idx, bool value) {
		m_dirty_subchunks_bitmap[subchunk_idx] = value;
	}

	inline bool has_dirty_subchunks() const {
		return m_dirty_subchunks_bitmap.any();
	}

	inline bool subchunk_exists(u32 idx) const {
		return m_subchunks[idx] != nullptr;
	}

	inline ChunkPosition get_position() const { 
		return m_position; 
	}

	vec3 get_global_position() const { 
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

	inline SubChunk *create_subchunk(u32 idx) {
		 m_subchunks[idx] = std::make_unique<SubChunk>(*this, idx);
		 return m_subchunks[idx].get();
	}

	inline SubChunk *get_or_create(u32 idx) {
		if(idx >= SUBCHUNK_COUNT) [[unlikely]] {
			std::println("Tried to get subchunk at invalid idx: {}", idx);
			return nullptr;
		}

		if(SubChunk *subchunk = get_subchunk(idx)) {
			return subchunk;
		}

		return create_subchunk(idx);
	}

private:
	ChunkPosition m_position;
	IWorld &m_world;

	std::bitset<SUBCHUNK_COUNT> m_dirty_subchunks_bitmap;
	std::array<std::unique_ptr<SubChunk>, SUBCHUNK_COUNT> m_subchunks;
};
