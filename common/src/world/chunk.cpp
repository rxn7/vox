#include "vox/common/world/chunk.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/world.hpp"
#include "vox/common/world/block_registry.hpp"
#include "vox/common/world/subchunk.hpp"

Chunk::Chunk(World &world, ChunkPosition position) 
: m_world(world), m_position(position) {
	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = SUBCHUNK_SIZE * 4 - 1;

			for(u32 y = 0; y < max_y; ++y) {
				if(y == max_y - 1) {
					set_block(LocalBlockPosition(x, y, z), BlockID::Grass);

					if(rand() % 20 == 0) {
						for(u32 i = 0; i < 5; ++i) {
							set_block(LocalBlockPosition(x, y + i, z), BlockID::Log);
						}
					}
				} else if(y < max_y - 5) {
					set_block(LocalBlockPosition(x, y, z), BlockID::Stone);
				} else {
					set_block(LocalBlockPosition(x, y, z), BlockID::Dirt);
				}
			}
		}
	}

	m_dirty_subchunks_bitmap.reset();
}

BlockID Chunk::get_block(LocalBlockPosition pos) const {
	PROFILE_FUNC();

	if(pos.y >= CHUNK_HEIGHT) {
		return BlockID::Air;
	}

	SubChunk *subchunk = get_subchunk(pos.y / SUBCHUNK_SIZE);
	if(subchunk == nullptr) {
		return BlockID::Air;
	}

	return subchunk->get_block({pos.x, pos.y % SUBCHUNK_SIZE, pos.z});
}

void Chunk::set_block(LocalBlockPosition pos, BlockID value) {
	PROFILE_FUNC();

	if(pos.y >= CHUNK_HEIGHT) {
		return;
	}

	const u32 subchunk_idx = pos.y / SUBCHUNK_SIZE;

	SubChunk *subchunk = get_subchunk(subchunk_idx);
	if(subchunk == nullptr) {
		if(value == BlockID::Air) {
			return;
		}

		m_subchunks[subchunk_idx] = std::make_unique<SubChunk>(*this, subchunk_idx);
		subchunk = m_subchunks[subchunk_idx].get();
	}

	subchunk->set_block({pos.x, pos.y % SUBCHUNK_SIZE, pos.z}, value);

	m_dirty_subchunks_bitmap[subchunk_idx] = true;
}

void Chunk::remove_subchunk(u32 idx) {
	PROFILE_FUNC();

	m_subchunks[idx] = nullptr;
	m_dirty_subchunks_bitmap[idx] = false;
}

bool Chunk::is_block_transparent(i8 x, i16 y, i8 z) const {
	PROFILE_FUNC();

	BlockID block;
	if(x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH) {
		const BlockPosition block_position(get_global_position() + vec3(x, y, z));
		block = m_world.get_block(block_position);
	} else {
		block = get_block(LocalBlockPosition(x, y, z));
	}

	if(block == BlockID::Air) {
		return true;
	}

	const BlockType &block_type = BlockRegistry::get(block);
	return block_type.is_transparent();
}

void Chunk::set_all_non_empty_subchunks_dirty() {
	PROFILE_FUNC();

	for(u32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		if(m_subchunks[i] != nullptr) {
			m_dirty_subchunks_bitmap[i] = true;
		}
	}
}
