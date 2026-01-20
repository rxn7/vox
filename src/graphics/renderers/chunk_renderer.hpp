#pragma once

#include "core/singleton.hpp"
#include "graphics/backend/shader.hpp"
#include "graphics/backend/texture_array.hpp"

struct DrawElementsIndirectCommand {
	u32 index_count;
	u32 instance_count;
	u32 first_index;
	i32 base_vertex;
	u32 base_instance;
};

class ChunkRenderer {
public:
	void init();
	void new_frame();

	void render(const mat4 &camera_matrix);

	void upload_mesh(u16 slot, std::span<const u32> vertices, std::span<const u32> indices);
	void add_draw_command(u16 slot, u32 index_count, i16vec3 chunk_position);

	u16 allocate_chunk_slot();
	void free_chunk_slot(u16 slot);
	
public:
	static constexpr u32 VERTEX_SLOT_SIZE = 16 * 16 * 16 * 6 * 4; // 8x8x8 chunk (worst case) * 6 faces * 4 vertices
	static constexpr u32 INDEX_SLOT_SIZE = VERTEX_SLOT_SIZE * 3 / 2;

	// TODO: Calculate on runtime based on render distance
	static constexpr u32 MAX_CHUNKS = 16;

	static constexpr u32 MAX_VERTICES = MAX_CHUNKS * VERTEX_SLOT_SIZE;
	static constexpr u32 MAX_INDICES =	MAX_CHUNKS * INDEX_SLOT_SIZE;

	bool m_use_wireframe = false;

private:
	u32 m_vao;
	u32 m_vbo;
	u32 m_ebo;

	u32 m_indirect_buffer;
	u32 m_packed_chunk_positions_ssbo;

	Shader m_shader;
	TextureArray m_textures;

	std::vector<DrawElementsIndirectCommand> m_draw_commands;
	std::vector<u32> m_packed_chunk_positions;

	std::vector<u16> m_free_chunk_slots;
	u16 m_next_chunk_slot = 0;
};
