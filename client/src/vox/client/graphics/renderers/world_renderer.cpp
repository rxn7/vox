#include "vox/common/world/chunk.hpp"
#include "vox/common/world/subchunk.hpp"
#include "vox/client/core/allocators/offset_allocator.hpp"
#include "vox/client/graphics/backend/packer.hpp"
#include "vox/client/graphics/renderers/world_renderer.hpp"
#include "vox/client/graphics/resources/texture_paths.hpp"

void WorldRenderer::init() {
	PROFILE_FUNC();
	

	m_shader.load(b::embed<"shaders/chunk-vert.glsl">().str(), b::embed<"shaders/chunk-frag.glsl">().str());
	m_textures.load(TexturePaths::get_all());

	glCreateVertexArrays(1, &m_vao);

	glCreateBuffers(1, &m_vbo);
	glNamedBufferStorage(m_vbo, MAX_VERTICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_ebo);
	glNamedBufferStorage(m_ebo, MAX_INDICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_indirect_buffer);
	glNamedBufferStorage(m_indirect_buffer, MAX_VISIBLE_SUBCHUNKS * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_packed_chunk_positions_ssbo);
	glNamedBufferStorage(m_packed_chunk_positions_ssbo, MAX_VISIBLE_SUBCHUNKS * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(u32));

	glVertexArrayAttribBinding(m_vao, 0, 0);
	glVertexArrayAttribIFormat(m_vao, 0, 1, GL_UNSIGNED_INT, 0);
	glEnableVertexArrayAttrib(m_vao, 0);

	glVertexArrayElementBuffer(m_vao, m_ebo);
}

void WorldRenderer::render(const mat4 &camera_matrix) {
	PROFILE_FUNC();

	m_draw_commands.clear();
	m_packed_subchunk_positions.clear();

	for(auto &mesh : m_subchunk_meshes) {
		render_subchunk_mesh(mesh.second);
	}

	if(m_draw_commands.empty()) {
		return;
	}

	glBindTextureUnit(0, m_textures.get_id());

	m_shader.bind();
	m_shader.set_uniform_i32("u_textures", 0);
	m_shader.set_uniform_mat4("u_camera_matrix", camera_matrix);

	glBindVertexArray(m_vao);

	glNamedBufferSubData(m_indirect_buffer, 0, m_draw_commands.size() * sizeof(DrawElementsIndirectCommand), m_draw_commands.data());
	glNamedBufferSubData(m_packed_chunk_positions_ssbo, 0, m_packed_subchunk_positions.size() * sizeof(u32), m_packed_subchunk_positions.data());	

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirect_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_packed_chunk_positions_ssbo);

	if(m_use_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} 

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, m_draw_commands.size(), 0);

	if(m_use_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
void WorldRenderer::update_subchunk(SubChunk &subchunk) {
	PROFILE_FUNC();

	const SubChunkPosition position = subchunk.get_position();

	const auto it = m_subchunk_meshes.find(position);
	if(it == m_subchunk_meshes.end()) {
		m_subchunk_meshes.emplace(position, SubChunkMesh(position));
	}

	PROFILE_FUNC();
	m_subchunk_meshes.at(position).generate_and_upload(subchunk, *this);
}

void WorldRenderer::remove_subchunk(SubChunk &subchunk) {
	const SubChunkPosition position = subchunk.get_position();

	const auto it = m_subchunk_meshes.find(position);
	if(it == m_subchunk_meshes.end()) {
		return;
	}

	SubChunkMesh &mesh = it->second;
	free_subchunk_mesh(mesh.m_alloc);

	PROFILE_FUNC();
	m_subchunk_meshes.erase(it);
}

void WorldRenderer::upload_subchunk_mesh(const SubChunkMeshAllocation &alloc, std::span<const u32> vertices, std::span<const u32> indices) {
	
	
	if(vertices.size() > alloc.vertex_alloc.m_size) [[unlikely]] {
		std::println("WorldRenderer::upload_chunk_mesh allocated {} vertices, trying to upload {}.", alloc.vertex_alloc.m_size, vertices.size());
		return;
	}
	
	if(indices.size() > alloc.index_alloc.m_size) [[unlikely]] {
		std::println("WorldRenderer::upload_chunk_mesh allocated {} indices, trying to upload {}.", alloc.index_alloc.m_size, indices.size());
		return;
	}
	
	const u64 vert_offset = alloc.vertex_alloc.m_offset * sizeof(u32);
	const u64 index_offset = alloc.index_alloc.m_offset * sizeof(u32);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, vert_offset, vertices.size_bytes(), vertices.data());

	PROFILE_FUNC();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, indices.size_bytes(), indices.data());
}

void WorldRenderer::render_subchunk_mesh(const SubChunkMesh &mesh) {
	

	DrawElementsIndirectCommand cmd = {
		.index_count = mesh.m_index_count,
		.instance_count = 1,
		.first_index = mesh.m_alloc.index_alloc.m_offset,
		.base_vertex = static_cast<i32>(mesh.m_alloc.vertex_alloc.m_offset),
		.base_instance = static_cast<u32>(m_draw_commands.size()),
	};
	PROFILE_FUNC();
	
	m_draw_commands.push_back(cmd);
	m_packed_subchunk_positions.push_back(Packer::pack_subchunk_position(mesh.get_position()));
}

std::optional<SubChunkMeshAllocation> WorldRenderer::allocate_subchunk_mesh(u32 vertex_count, u32 index_count) {
	
	
	std::optional<OffsetAllocator::Allocation> vertex_alloc = m_vertex_allocator.allocate(vertex_count);
	if(!vertex_alloc.has_value()) {
		std::println("WorldRenderer::allocate_chunks_mesh: not enough space for {} vertices", vertex_count);
		return std::nullopt;
	}
	
	std::optional<OffsetAllocator::Allocation> index_alloc = m_index_allocator.allocate(index_count);
	if(!index_alloc.has_value()) {
		std::println("WorldRenderer::allocate_chunks_mesh: not enough space for {} indices", index_count);
		return std::nullopt;
	}
	
	return SubChunkMeshAllocation{
		.vertex_alloc = *vertex_alloc,
		.index_alloc = *index_alloc
	};
}

void WorldRenderer::free_subchunk_mesh(const SubChunkMeshAllocation &alloc) {
	if(alloc.vertex_alloc.m_size > 0) {
		m_vertex_allocator.free(alloc.vertex_alloc);
	}
	
	if(alloc.index_alloc.m_size > 0) {
		m_index_allocator.free(alloc.index_alloc);
	}
}
