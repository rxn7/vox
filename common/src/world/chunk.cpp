#include "vox/common/world/chunk.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk_position.hpp"
#include "vox/common/world/world.hpp"
#include "vox/common/world/subchunk.hpp"

Chunk::Chunk(World &world, ChunkPosition position) : m_world(world), m_position(position) {
	m_dirty_subchunks_bitmap.reset();
}

BlockID Chunk::get_block_local(LocalBlockPosition pos) const {
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

BlockID Chunk::get_block_relative(i8 x, i16 y, i8 z) const {
	PROFILE_FUNC();

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

void Chunk::set_block_relative(i8 x, i16 y, i8 z, BlockID value) {
	PROFILE_FUNC();

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

void Chunk::generate() {
	PROFILE_FUNC();

	// TODO: Temporary
	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = SUBCHUNK_SIZE * 4 - 1;

			for(u32 y = 0; y < max_y; ++y) {
				if(y == max_y - 1) {
					set_block_local(LocalBlockPosition(x, y, z), BlockID::Grass);

					// Trees (temporary)
					if(rand() % 20 == 0) {
						bool has_neighbor = false;
						for(i8 dx = -1; dx <= 1; ++dx) {
							for(i8 dz = -1; dz <= 1; ++dz) {
								if(dx == 0 && dz == 0) {
									continue;
								}

								if(get_block_relative(x + dx, y+1, z + dz) != BlockID::Air) {
									has_neighbor = true;
									break;
								}
							}
						}

						if(has_neighbor) {
							continue;
						}

						const u8 tree_height = 4 + rand() % 2;

						i8 radius = 2;
						for(u32 dy = tree_height-1; dy <= tree_height + 1; ++dy) {
							for(i8 dx = -radius; dx <= radius; ++dx) {
								for(i8 dz = -radius; dz <= radius; ++dz) {
									set_block_relative(x + dx, y + dy, z + dz, BlockID::Leaves);
								}
							}

							radius = glm::max(0, radius - 1);
						}

						for(u32 i = 0; i < tree_height - 1; ++i) {
							set_block_local(LocalBlockPosition(x, y + i, z), BlockID::Log);
						}
					}
				} else if(y < max_y - 5) {
					set_block_local(LocalBlockPosition(x, y, z), BlockID::Stone);
				} else {
					set_block_local(LocalBlockPosition(x, y, z), BlockID::Dirt);
				}
			}
		}
	}
}
