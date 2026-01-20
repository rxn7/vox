#include "world/chunk.hpp"
#include "world/world.hpp"
#include "world/block_registry.hpp"
#include "graphics/backend/packer.hpp"
#include "graphics/resources/texture_id.hpp"
#include "graphics/renderers/chunk_renderer.hpp"

Chunk::Chunk(World &world, ivec3 position) 
: m_world(world), m_position(position) {
	memset(m_blocks.data(), 0, TOTAL_BLOCKS);

	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = rand() % (CHUNK_WIDTH - 1) + 1;

			for(u32 y = 0; y < max_y; ++y) {
				if(y == max_y - 1) {
					set_block(x, y, z, BlockID::Grass);
				} else if(y < 10) {
					set_block(x, y, z, BlockID::Stone);
				} else {
					set_block(x, y, z, BlockID::Dirt);
				}
			}
		}
	}

	m_is_dirty = true;
}

void Chunk::render(ChunkRenderer &renderer) {
	PROFILE_FUNC();

	if(m_is_dirty) {
		generate_mesh_and_upload(renderer);
		m_is_dirty = false;
	}

	if(m_render_slot.has_value() && m_index_count > 0) {
		renderer.add_draw_command(m_render_slot.value(), m_index_count, m_position);
	}
}

void Chunk::generate_mesh_and_upload(ChunkRenderer &renderer) {
	PROFILE_FUNC();

	static std::vector<u32> s_tmp_vertices;
	static std::vector<u32> s_tmp_indices;

	s_tmp_vertices.reserve(ChunkRenderer::VERTEX_SLOT_SIZE);
	s_tmp_vertices.clear();

	s_tmp_indices.reserve(ChunkRenderer::INDEX_SLOT_SIZE);
	s_tmp_indices.clear();

	u32 index_offset = 0;

	for(u8 y = 0; y < CHUNK_WIDTH; ++y) {
		for(u8 z = 0; z < CHUNK_WIDTH; ++z) {
			for(u8 x = 0; x < CHUNK_WIDTH; ++x) {
				const BlockID block = get_block(x, y, z);

				if(block == BlockID::Air) {
					continue;
				}

				const BlockType &block_type = BlockRegistry::get(block);

				const auto add_quad = [&](
					FaceID face_id,
					u8 x0, u8 y0, u8 z0,  // Bottom-Left
					u8 x1, u8 y1, u8 z1,  // Bottom-Right
					u8 x2, u8 y2, u8 z2,  // Top-Right
					u8 x3, u8 y3, u8 z3)  // Top-Left
				{

					const TextureID texture_id = block_type.get_texture_id(face_id);

					s_tmp_vertices.push_back(Packer::pack_vertex(x + x0, y + y0, z + z0, texture_id, face_id));
					s_tmp_vertices.push_back(Packer::pack_vertex(x + x1, y + y1, z + z1, texture_id, face_id));
					s_tmp_vertices.push_back(Packer::pack_vertex(x + x2, y + y2, z + z2, texture_id, face_id));
					s_tmp_vertices.push_back(Packer::pack_vertex(x + x3, y + y3, z + z3, texture_id, face_id));
					
					// Standard Quad Indices (0-1-2, 2-3-0)
					s_tmp_indices.push_back(index_offset + 0);
					s_tmp_indices.push_back(index_offset + 1);
					s_tmp_indices.push_back(index_offset + 2);
					s_tmp_indices.push_back(index_offset + 2);
					s_tmp_indices.push_back(index_offset + 3);
					s_tmp_indices.push_back(index_offset + 0);
					
					index_offset += 4;
				};

				if(should_draw_face(x + 1, y, z))
					add_quad(FaceID::Right,  1, 0, 1,   1, 0, 0,   1, 1, 0,   1, 1, 1);

				if(should_draw_face(x - 1, y, z))
					add_quad(FaceID::Left,   0, 0, 0,   0, 0, 1,   0, 1, 1,   0, 1, 0);

				if(should_draw_face(x, y + 1, z))
					add_quad(FaceID::Top,    0, 1, 1,   1, 1, 1,   1, 1, 0,   0, 1, 0);

				if(should_draw_face(x, y - 1, z))
					add_quad(FaceID::Bottom, 0, 0, 0,   1, 0, 0,   1, 0, 1,   0, 0, 1);

				if(should_draw_face(x, y, z + 1))
					add_quad(FaceID::Front,  0, 0, 1,   1, 0, 1,   1, 1, 1,   0, 1, 1);

				if(should_draw_face(x, y, z - 1))
					add_quad(FaceID::Back,   1, 0, 0,   0, 0, 0,   0, 1, 0,   1, 1, 0);
			}
		}
	}

	if(!m_render_slot.has_value()) {
		m_render_slot = renderer.allocate_chunk_slot();
	}

	m_index_count = s_tmp_indices.size();
	renderer.upload_mesh(m_render_slot.value(), s_tmp_vertices, s_tmp_indices);
}

bool Chunk::should_draw_face(i8 x, i8 y, i8 z) const {
	PROFILE_FUNC();

	BlockID block;

	if(x < 0 || x >= static_cast<i8>(CHUNK_WIDTH) 
		|| y < 0 || y >= static_cast<i8>(CHUNK_WIDTH) 
		|| z < 0 || z >= static_cast<i8>(CHUNK_WIDTH)
	) {
		const ivec3 chunk_offset(
            x < 0 ? -1 : (x >= static_cast<i8>(CHUNK_WIDTH) ? 1 : 0),
            y < 0 ? -1 : (y >= static_cast<i8>(CHUNK_WIDTH) ? 1 : 0),
            z < 0 ? -1 : (z >= static_cast<i8>(CHUNK_WIDTH) ? 1 : 0)
        );

		const Chunk *chunk = m_world.get_chunk(m_position + chunk_offset);
		if(chunk == nullptr) {
			return true;
		}

		const u8 local_x = (x + CHUNK_WIDTH) % CHUNK_WIDTH;
        const u8 local_y = (y + CHUNK_WIDTH) % CHUNK_WIDTH;
        const u8 local_z = (z + CHUNK_WIDTH) % CHUNK_WIDTH;

		block = chunk->get_block(local_x, local_y, local_z);
	} else {
		block = get_block(x, y, z);
	}

	if(block == BlockID::Air) {
		return true;
	}

	const BlockType &block_type = BlockRegistry::get(block);
	return block_type.is_transparent;
}
