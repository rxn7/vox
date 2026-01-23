#include "vox/common/world/block_registry.hpp"
#include "vox/engine/graphics/face_data.hpp"
#include "vox/engine/graphics/backend/packer.hpp"
#include "vox/engine/graphics/backend/chunk_mesh.hpp"
#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/engine/graphics/graphics_settings.hpp"

inline u8 calculate_vertex_ao(bool side1_occluded, bool side2_occluded, bool corner_occluded) {
    if(side1_occluded && side2_occluded) {
        return 3;
    }
    return side1_occluded + side2_occluded + corner_occluded;
}

void ChunkMesh::generate_and_upload(const Chunk &chunk, WorldRenderer &renderer) {
	PROFILE_FUNC();
    
	static std::vector<u32> s_vertices{};
	s_vertices.reserve(4096);
	s_vertices.clear();

	static std::vector<u32> s_indices;
	s_vertices.reserve(6144);
	s_indices.clear();

	if(m_alloc.is_valid()) {
		renderer.free_chunk_mesh(m_alloc);
	}

    const bool ao_enabled = GraphicsSettings::get_instance().m_state.ambient_occlusion_enabled;

	u32 index_offset = 0;
	for(i8 y = 0; y < CHUNK_WIDTH; ++y) {
		for(i8 z = 0; z < CHUNK_WIDTH; ++z) {
			for(i8 x = 0; x < CHUNK_WIDTH; ++x) {
				const BlockID block = chunk.get_block({x, y, z});

				if(block == BlockID::Air) {
					continue;
				}

				const BlockType &block_type = BlockRegistry::get(block);

				const auto add_quad = [&](BlockFaceID face_id, u8vec3 bl, u8vec3 br, u8vec3 tr, u8vec3 tl) {
					const TextureID texture_id = block_type.get_texture_id(face_id);
					const auto [normal, tangent1, tangent2] = get_face_data(face_id);

					const auto is_block_solid = [&](i8vec3 offset) -> bool {
						return !chunk.is_block_transparent(x + offset.x, y + offset.y, z + offset.z);
					};
                    
					const auto calculate_ao = [&](i8 t1, i8 t2) -> u8 {
						PROFILE_SCOPE("Ambient Occlusion");

                        if(!ao_enabled) {
                            return 0; 
                        }

						const i8vec3 side1 = normal + tangent1 * t1;
						const i8vec3 side2 = normal + tangent2 * t2;
						const i8vec3 corner = normal + tangent1 * t1 + tangent2 * t2;

						return calculate_vertex_ao(
							is_block_solid(side1),
							is_block_solid(side2),
							is_block_solid(corner)
						);
					};

					const u8 ao_bl = calculate_ao(-1, -1);
					const u8 ao_br = calculate_ao( 1, -1);
					const u8 ao_tr = calculate_ao( 1,  1);
					const u8 ao_tl = calculate_ao(-1,  1);

					s_vertices.push_back(Packer::pack_vertex(x + bl.x, y + bl.y, z + bl.z, texture_id, face_id, ao_bl));
					s_vertices.push_back(Packer::pack_vertex(x + br.x, y + br.y, z + br.z, texture_id, face_id, ao_br));
					s_vertices.push_back(Packer::pack_vertex(x + tr.x, y + tr.y, z + tr.z, texture_id, face_id, ao_tr));
					s_vertices.push_back(Packer::pack_vertex(x + tl.x, y + tl.y, z + tl.z, texture_id, face_id, ao_tl));
					
					s_indices.push_back(index_offset + 0);
					s_indices.push_back(index_offset + 1);
					s_indices.push_back(index_offset + 2);
					s_indices.push_back(index_offset + 2);
					s_indices.push_back(index_offset + 3);
					s_indices.push_back(index_offset + 0);
					
					index_offset += 4;
				};

				if(chunk.is_block_transparent(x + 1, y, z))
					add_quad(BlockFaceID::Right,		{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1});

				if(chunk.is_block_transparent(x - 1, y, z))
					add_quad(BlockFaceID::Left,		{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0});

				if(chunk.is_block_transparent(x, y + 1, z))
					add_quad(BlockFaceID::Top,		{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0});

				if(chunk.is_block_transparent(x, y - 1, z))
					add_quad(BlockFaceID::Bottom,	{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1});

				if(chunk.is_block_transparent(x, y, z + 1))
					add_quad(BlockFaceID::Front,		{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1});

				if(chunk.is_block_transparent(x, y, z - 1))
					add_quad(BlockFaceID::Back,		{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0});
			}
		}
	}

    const auto result = renderer.allocate_chunk_mesh(s_vertices.size(), s_indices.size());
    if(result) {
        m_alloc = *result;
        renderer.upload_chunk_mesh(m_alloc, s_vertices, s_indices);
    }

    m_index_count = s_indices.size();
}
