#include "vox/common/world/subchunk.hpp"
#include "vox/common/world/chunk.hpp"

SubChunk::SubChunk(Chunk &chunk, u32 idx) : m_chunk(chunk), m_idx(idx)  {
	m_blocks.fill(BlockID::Air);
}

void SubChunk::set_blocks(const SubChunkData &blocks) {
	std::copy(blocks.begin(), blocks.end(), m_blocks.begin());
}

bool SubChunk::is_block_transparent(i8 x, i16 y, i8 z) const {
	return m_chunk.is_block_transparent_relative(x, static_cast<i16>(y) + m_idx * SUBCHUNK_SIZE, z);
}

SubChunkPosition SubChunk::get_position() const {
	return SubChunkPosition(m_chunk.get_position().x, m_idx, m_chunk.get_position().y);
}

bool SubChunk::is_empty() const {
	PROFILE_FUNC();
	for(u32 i = 0; i < SUBCHUNK_VOLUME; ++i) {
		if(m_blocks[i] != BlockID::Air) {
			return false;
		}
	}
	return true;
}
