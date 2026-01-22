#include "vox/world/chunk.hpp"
#include "vox/graphics/chunk_mesh_generator.hpp"
#include "vox/world/world.hpp"
#include "vox/world/block_registry.hpp"
#include "vox/graphics/renderers/world_renderer.hpp"

Chunk::Chunk(World &world, ivec3 position) 
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

	m_is_dirty = true;
}

void Chunk::render(WorldRenderer &renderer) {
	PROFILE_FUNC();

	if(m_is_dirty) {
		m_index_count = ChunkMeshGenerator::generate_mesh_and_upload(*this, m_alloc, renderer);
		m_is_dirty = false;
	}

	if(m_alloc.is_valid() && m_index_count > 0) {
		renderer.add_draw_command(m_alloc, m_index_count, m_position);
	}
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
	return block_type.is_transparent;
}
