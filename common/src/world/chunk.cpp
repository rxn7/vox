#include "vox/common/world/chunk.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/i_world.hpp"
#include "vox/common/world/subchunk.hpp"

Chunk::Chunk(IWorld &world, ChunkPosition position) : m_world(world), m_position(position) {
	m_dirty_subchunks_bitmap.reset();
}

BlockID Chunk::get_block_local(LocalBlockPosition pos) const {
	if(pos.y >= CHUNK_HEIGHT) {
		return BlockID::Air;
	}

	std::shared_ptr<SubChunk> p_subchunk = get_subchunk(pos.y / SUBCHUNK_SIZE);
	if(p_subchunk == nullptr) {
		return BlockID::Air;
	}

	return p_subchunk->get_block({pos.x, pos.y % SUBCHUNK_SIZE, pos.z});
}

BlockID Chunk::get_block_relative(i8 x, i16 y, i8 z) const {
	BlockID block;
	if(x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH) {
		const BlockPosition block_position(get_global_position() + vec3(x, y, z));
		block = m_world.get_block(block_position);
	} else {
		block = get_block_local(LocalBlockPosition(x, y, z));
	}

	return block;
}

void Chunk::set_block_local(LocalBlockPosition pos, BlockID value) {
	if(pos.y >= CHUNK_HEIGHT) {
		return;
	}

	const u32 subchunk_idx = pos.y / SUBCHUNK_SIZE;

	std::shared_ptr<SubChunk> p_subchunk = get_subchunk(subchunk_idx);
	if(p_subchunk == nullptr) {
		if(value == BlockID::Air) {
			return;
		}

		p_subchunk = m_subchunks[subchunk_idx] = m_world.create_subchunk(m_ref, subchunk_idx);
	}

	p_subchunk->set_block({pos.x, pos.y % SUBCHUNK_SIZE, pos.z}, value);

	m_dirty_subchunks_bitmap[subchunk_idx] = true;
}

void Chunk::set_block_relative(i8 x, i16 y, i8 z, BlockID value) {
	BlockID block;
	if(x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH) {
		const BlockPosition block_position(get_global_position() + vec3(x, y, z));
		m_world.set_block(block_position, value);
	} else {
		set_block_local(LocalBlockPosition(x, y, z), value);
	}
}

void Chunk::remove_subchunk(u32 idx) {
	PROFILE_FUNC();

	m_subchunks[idx] = nullptr;
	m_dirty_subchunks_bitmap[idx] = false;
}

void Chunk::set_all_non_empty_subchunks_dirty() {
	PROFILE_FUNC();

	for(u32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		if(m_subchunks[i] != nullptr) {
			m_dirty_subchunks_bitmap[i] = true;
		}
	}
}
