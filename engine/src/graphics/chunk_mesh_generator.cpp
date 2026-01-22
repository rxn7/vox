#include "vox/graphics/chunk_mesh_generator.hpp"
#include "vox/graphics/backend/packer.hpp"
#include "vox/graphics/face_data.hpp"
#include "vox/graphics/graphics_settings.hpp"
#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"
#include "vox/world/block_registry.hpp"
#include "vox/world/block_type.hpp"

u32 ChunkMeshGenerator::generate_mesh_and_upload(const Chunk &chunk, ChunkMeshAllocation &alloc, WorldRenderer &renderer) {
	PROFILE_FUNC();
    
    if(alloc.is_valid()) {
        renderer.free_chunk_mesh(alloc);
    }

	static std::vector<u32> s_vertices{};
	s_vertices.reserve(4096);
	s_vertices.clear();

	static std::vector<u32> s_indices;
	s_vertices.reserve(6144);
	s_indices.clear();

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

				const auto add_quad = [&](FaceID face_id, u8vec3 bl, u8vec3 br, u8vec3 tr, u8vec3 tl) {
					const TextureID texture_id = block_type.get_texture_id(face_id);
					const auto [normal, tangent1, tangent2] = get_face_data(face_id);

					const auto is_block_solid = [&](i8vec3 offset) -> bool {
						return !chunk.is_block_transparent(x + offset.x, y + offset.y, z + offset.z);
					};
                    
					const auto calculate_ao = [&](i8 t1, i8 t2) -> u8 {
                        PROFILE_FUNC();

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
					add_quad(FaceID::Right,		{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1});

				if(chunk.is_block_transparent(x - 1, y, z))
					add_quad(FaceID::Left,		{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0});

				if(chunk.is_block_transparent(x, y + 1, z))
					add_quad(FaceID::Top,		{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0});

				if(chunk.is_block_transparent(x, y - 1, z))
					add_quad(FaceID::Bottom,	{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1});

				if(chunk.is_block_transparent(x, y, z + 1))
					add_quad(FaceID::Front,		{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1});

				if(chunk.is_block_transparent(x, y, z - 1))
					add_quad(FaceID::Back,		{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0});
			}
		}
	}

    const auto result = renderer.allocate_chunk_mesh(s_vertices.size(), s_indices.size());
    if(result) {
        alloc = *result;
        renderer.upload_chunk_mesh(alloc, s_vertices, s_indices);
    }

    return s_indices.size();
}