#include "core/chunk.hpp"
#include "core/block_registry.hpp"
#include "core/texture_id.hpp"
#include "core/render/chunk_renderer.hpp"
#include "core/render/voxel.hpp"
#include "core/render/packer.hpp"

Chunk::Chunk() {
	memset(m_blocks.data(), 0, TOTAL_BLOCKS);

	for(u32 y = 0; y < CHUNK_WIDTH; y += 2) {
		for(u32 x = 0; x < CHUNK_WIDTH; x += 2) {
			for(u32 z = 0; z < CHUNK_WIDTH; z += 2) {
				set_block(x, y, z, (BlockID)(rand() % (size_t)BlockID::SIZE));
			}
		}
	}

	m_is_dirty = true;
}

void Chunk::update() {
	PROFILE_FUNC();

	if(m_is_dirty) {
		generate_mesh_and_upload();
		m_is_dirty = false;
	}
}

void Chunk::generate_mesh_and_upload() {
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

				add_quad(FaceID::Right,  1, 0, 1,   1, 0, 0,   1, 1, 0,   1, 1, 1);
				add_quad(FaceID::Left,   0, 0, 0,   0, 0, 1,   0, 1, 1,   0, 1, 0);
				add_quad(FaceID::Top,    0, 1, 1,   1, 1, 1,   1, 1, 0,   0, 1, 0);
				add_quad(FaceID::Bottom, 0, 0, 0,   1, 0, 0,   1, 0, 1,   0, 0, 1);
				add_quad(FaceID::Front,  0, 0, 1,   1, 0, 1,   1, 1, 1,   0, 1, 1);
				add_quad(FaceID::Back,   1, 0, 0,   0, 0, 0,   0, 1, 0,   1, 1, 0);
			}
		}
	}

	if(!m_render_slot.has_value()) {
		m_render_slot = ChunkRenderer::get_instance().allocate_chunk_slot();
	}

	m_index_count = s_tmp_indices.size();
	ChunkRenderer::get_instance().upload_mesh(m_render_slot.value(), s_tmp_vertices, s_tmp_indices);
}
