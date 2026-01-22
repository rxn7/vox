#include "vox/world/chunk.hpp"
#include "vox/graphics/face_data.hpp"
#include "vox/graphics/graphics_settings.hpp"
#include "vox/world/world.hpp"
#include "vox/world/block_registry.hpp"
#include "vox/graphics/backend/packer.hpp"
#include "vox/graphics/resources/texture_id.hpp"
#include "vox/graphics/renderers/world_renderer.hpp"

Chunk::Chunk(World &world, ivec3 position) 
: m_world(world), m_position(position) {
	memset(m_blocks.data(), 0, TOTAL_BLOCKS);

	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = rand() % (4) + 12;

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
		generate_mesh_and_upload(renderer);
		m_is_dirty = false;
	}

	if(m_alloc.is_valid() && m_index_count > 0) {
		renderer.add_draw_command(m_alloc, m_index_count, m_position);
	}
}

void Chunk::generate_mesh_and_upload(WorldRenderer &renderer) {
	PROFILE_FUNC();
    
    if(m_alloc.is_valid()) {
        renderer.free_chunk_mesh(m_alloc);
    }

	static std::vector<u32> s_vertices{};
	s_vertices.reserve(4096);
	s_vertices.clear();

	static std::vector<u32> s_indices;
	s_vertices.reserve(6144);
	s_indices.clear();

	u32 index_offset = 0;

	for(i8 y = 0; y < CHUNK_WIDTH; ++y) {
		for(i8 z = 0; z < CHUNK_WIDTH; ++z) {
			for(i8 x = 0; x < CHUNK_WIDTH; ++x) {
				const BlockID block = get_block({x, y, z});

				if(block == BlockID::Air) {
					continue;
				}

				const BlockType &block_type = BlockRegistry::get(block);

				const auto add_quad = [&](FaceID face_id, u8vec3 bl, u8vec3 br, u8vec3 tr, u8vec3 tl) {
					const TextureID texture_id = block_type.get_texture_id(face_id);
					const auto [normal, tangent1, tangent2] = get_face_data(face_id);

					const auto is_block_solid = [&](i8vec3 offset) -> bool {
						return !is_block_transparent(x + offset.x, y + offset.y, z + offset.z);
					};
                    
                    const bool ao_enabled = GraphicsSettings::get_instance().m_state.ambient_occlusion_enabled;

					const auto calculate_ao = [&](i8 t1, i8 t2) -> u8 {
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

				if(is_block_transparent(x + 1, y, z))
					add_quad(FaceID::Right,		{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1});

				if(is_block_transparent(x - 1, y, z))
					add_quad(FaceID::Left,		{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0});

				if(is_block_transparent(x, y + 1, z))
					add_quad(FaceID::Top,		{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0});

				if(is_block_transparent(x, y - 1, z))
					add_quad(FaceID::Bottom,	{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1});

				if(is_block_transparent(x, y, z + 1))
					add_quad(FaceID::Front,		{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1});

				if(is_block_transparent(x, y, z - 1))
					add_quad(FaceID::Back,		{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0});
			}
		}
	}

    m_index_count = s_indices.size();
    
    const auto result = renderer.allocate_chunk_mesh(s_vertices.size(), s_indices.size());
    if(result) {
        m_alloc = *result;
        renderer.upload_chunk_mesh(m_alloc, s_vertices, s_indices);
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

u8 Chunk::calculate_vertex_ao(bool side1_occluded, bool side2_occluded, bool corner_occluded) const {
	if(side1_occluded && side2_occluded) {
		return 3;
	}

	return side1_occluded + side2_occluded + corner_occluded;
}
