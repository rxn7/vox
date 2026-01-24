#include "vox/common/world/chunk.hpp"
#include "vox/common/world/world.hpp"
#include "vox/common/world/block_registry.hpp"

Chunk::Chunk(World &world, ChunkPosition position) 
: m_world(world), m_position(position) {
	memset(m_blocks.data(), 0, TOTAL_BLOCKS);

	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = 16;

			for(u32 y = 0; y < max_y; ++y) {
				if(y == max_y - 1) {
					set_block({x, y, z}, BlockID::Grass);
				} else if(y < 10) {
					set_block({x, y, z}, BlockID::Stone);
				} else {
					set_block({x, y, z}, BlockID::Dirt);
				}
			}
		}
	}
    
    set_dirty(true);
}

bool Chunk::is_block_transparent(i8 x, i8 y, i8 z) const {
	PROFILE_FUNC();

	BlockID block;
	if(x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH) {
		const BlockPosition block_position(get_global_position() + vec3(x, y, z));
		block = m_world.get_block(block_position);
	} else {
		block = get_block({x, y, z});
	}

	if(block == BlockID::Air) {
		return true;
	}

	const BlockType &block_type = BlockRegistry::get(block);
	return block_type.m_is_transparent;
}
