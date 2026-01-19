#include "core/render/chunk_renderer.hpp"
#include "core/render/packer.hpp"
#include "core/texture_paths.hpp"

SINGLETON_IMPL(ChunkRenderer);

void ChunkRenderer::init() {
	m_shader.load(b::embed<"shaders/chunk-vert.glsl">().str(), b::embed<"shaders/chunk-frag.glsl">().str());
	m_textures.load(TexturePaths::get_all());

	glCreateVertexArrays(1, &m_vao);

	glCreateBuffers(1, &m_vbo);
	glNamedBufferStorage(m_vbo, MAX_VERTICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_ebo);
	glNamedBufferStorage(m_ebo, MAX_INDICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_indirect_buffer);
	glNamedBufferStorage(m_indirect_buffer, MAX_CHUNKS * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_packed_chunk_positions_ssbo);
	glNamedBufferStorage(m_packed_chunk_positions_ssbo, MAX_CHUNKS * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(u32));

	glVertexArrayAttribBinding(m_vao, 0, 0);
	glVertexArrayAttribIFormat(m_vao, 0, 1, GL_UNSIGNED_INT, 0);
	glEnableVertexArrayAttrib(m_vao, 0);

	glVertexArrayElementBuffer(m_vao, m_ebo);
}

void ChunkRenderer::new_frame() {
	// TODO: do this only when chunks change
	m_draw_commands.clear();
	m_packed_chunk_positions.clear();
}

void ChunkRenderer::render(const mat4 &camera_matrix) {
	PROFILE_FUNC();

	if(m_draw_commands.empty()) {
		return;
	}

	glBindTextureUnit(0, m_textures.get_id());

	m_shader.bind();
	m_shader.set_uniform_i32("u_textures", 0);
	m_shader.set_uniform_mat4("u_camera_matrix", camera_matrix);

	glBindVertexArray(m_vao);

	glNamedBufferSubData(m_indirect_buffer, 0, m_draw_commands.size() * sizeof(DrawElementsIndirectCommand), m_draw_commands.data());
	glNamedBufferSubData(m_packed_chunk_positions_ssbo, 0, m_packed_chunk_positions.size() * sizeof(u32), m_packed_chunk_positions.data());	

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirect_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_packed_chunk_positions_ssbo);

	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} 

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, m_draw_commands.size(), 0);

	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void ChunkRenderer::upload_mesh(u16 slot, std::span<const u32> vertices, std::span<const u32> indices) {
	PROFILE_FUNC();

	u32 vert_count = static_cast<u32>(vertices.size());
	if(vertices.size() > VERTEX_SLOT_SIZE) [[unlikely]] {
		std::println("ChunkRenderer: Too many vertices to upload mesh ({} > {})", vertices.size(), VERTEX_SLOT_SIZE);
		vert_count = VERTEX_SLOT_SIZE;
	}

	u32 index_count = static_cast<u32>(indices.size());
	if(indices.size() > INDEX_SLOT_SIZE) [[unlikely]] {
		std::println("ChunkRenderer: Too many indices to upload mesh ({} > {})", indices.size(), INDEX_SLOT_SIZE);
		index_count = INDEX_SLOT_SIZE;
	}

	const u64 vert_offset = slot * VERTEX_SLOT_SIZE;
	const u64 index_offset = slot * INDEX_SLOT_SIZE;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, vert_offset * sizeof(u32), vert_count * sizeof(u32), vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset * sizeof(u32), index_count * sizeof(u32), indices.data());
}

void ChunkRenderer::add_draw_command(u16 slot, u32 index_count, i16vec3 chunk_position) {
	PROFILE_FUNC();

	if(m_draw_commands.size() >= MAX_CHUNKS) [[unlikely]] {
		std::println("ChunkRenderer: Too many chunks ({} > {})", m_draw_commands.size(), MAX_CHUNKS);
		return;
	}

	DrawElementsIndirectCommand cmd = {
		.index_count = index_count,
		.instance_count = 1,
		.first_index = slot * INDEX_SLOT_SIZE,
		.base_vertex = static_cast<i32>(slot * VERTEX_SLOT_SIZE),
		.base_instance = static_cast<u32>(m_draw_commands.size()),
	};
	
	m_draw_commands.push_back(cmd);
	m_packed_chunk_positions.push_back(Packer::pack_chunk_position(chunk_position));
}

u16 ChunkRenderer::allocate_chunk_slot() {
	PROFILE_FUNC();

	if(!m_free_chunk_slots.empty()) {
		const u16 slot = m_free_chunk_slots.back();
		m_free_chunk_slots.pop_back();
		return slot;
	}

	return m_next_chunk_slot++;
}

void ChunkRenderer::free_chunk_slot(u16 slot) {
	m_free_chunk_slots.push_back(slot);
}
