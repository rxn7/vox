#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/world/world_constants.hpp"

struct SubChunk {
	SubChunk(Chunk &chunk, u32 idx) : m_chunk(chunk), m_idx(idx) {
		m_blocks.fill(BlockID::Air);
	}

	inline BlockID get_block(ivec3 v) const { 
		return m_blocks[v.y * SUBCHUNK_SIZE * SUBCHUNK_SIZE + v.z * SUBCHUNK_SIZE + v.x];
	}

	inline void set_block(ivec3 pos, BlockID value) {
		m_blocks[pos.y * SUBCHUNK_SIZE * SUBCHUNK_SIZE + pos.z * SUBCHUNK_SIZE + pos.x] = value;
	}

	inline bool is_empty() const {
		PROFILE_FUNC();
		for(u32 i = 0; i < SUBCHUNK_VOLUME; ++i) {
			if(m_blocks[i] != BlockID::Air) {
				return false;
			}
		}
		return true;
	}

	inline bool is_block_transparent(i8 x, i16 y, i8 z) const {
		return m_chunk.is_block_transparent(x, static_cast<i16>(y) + m_idx * SUBCHUNK_SIZE, z);
	}

	inline SubChunkPosition get_position() const {
		return SubChunkPosition(m_chunk.get_position().x, m_idx, m_chunk.get_position().y);
	}

	Chunk &m_chunk;
	u32 m_idx;
	std::array<BlockID, SUBCHUNK_VOLUME> m_blocks;
};
