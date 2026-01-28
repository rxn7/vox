#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/world_constants.hpp"
#include "vox/common/world/chunk_position.hpp"

using SubChunkData = std::array<BlockID, SUBCHUNK_VOLUME>;

class Chunk;

class SubChunk {
public:
	SubChunk(Chunk &chunk, u32 idx);

	bool is_empty() const;

	bool is_block_transparent(i8 x, i16 y, i8 z) const;

	SubChunkPosition get_position() const;

	void set_blocks(const SubChunkData &blocks);

	inline BlockID get_block(ivec3 v) const { 
		return m_blocks[v.y * SUBCHUNK_SIZE * SUBCHUNK_SIZE + v.z * SUBCHUNK_SIZE + v.x];
	}

	inline void set_block(ivec3 pos, BlockID value) {
		m_blocks[pos.y * SUBCHUNK_SIZE * SUBCHUNK_SIZE + pos.z * SUBCHUNK_SIZE + pos.x] = value;
	}

	inline u32 get_idx() const {
		return m_idx;
	}

	inline Chunk &get_chunk() const {
		return m_chunk;
	}

	inline SubChunkData &get_blocks() {
		return m_blocks;
	}

private:
	Chunk &m_chunk;
	u32 m_idx;
	SubChunkData m_blocks;
};
