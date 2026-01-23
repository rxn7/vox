#include "vox/common/world/chunk.hpp"
#include "vox/engine/core/allocators/offset_allocator.hpp"
#include "vox/engine/graphics/backend/packer.hpp"
#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/engine/graphics/resources/texture_paths.hpp"

void WorldRenderer::init() {
	m_shader.load(b::embed<"shaders/chunk-vert.glsl">().str(), b::embed<"shaders/chunk-frag.glsl">().str());
	m_textures.load(TexturePaths::get_all());

	glCreateVertexArrays(1, &m_vao);

	glCreateBuffers(1, &m_vbo);
	glNamedBufferStorage(m_vbo, MAX_VERTICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_ebo);
	glNamedBufferStorage(m_ebo, MAX_INDICES * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_indirect_buffer);
	glNamedBufferStorage(m_indirect_buffer, MAX_VISIBLE_CHUNKS * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &m_packed_chunk_positions_ssbo);
	glNamedBufferStorage(m_packed_chunk_positions_ssbo, MAX_VISIBLE_CHUNKS * sizeof(u32), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(u32));

	glVertexArrayAttribBinding(m_vao, 0, 0);
	glVertexArrayAttribIFormat(m_vao, 0, 1, GL_UNSIGNED_INT, 0);
	glEnableVertexArrayAttrib(m_vao, 0);

	glVertexArrayElementBuffer(m_vao, m_ebo);
}

void WorldRenderer::render(const mat4 &camera_matrix) {
	PROFILE_FUNC();

	m_draw_commands.clear();
	m_packed_chunk_positions.clear();

	for(auto &mesh : m_chunk_meshes) {
		render_chunk_mesh(mesh.second);
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
	glNamedBufferSubData(m_packed_chunk_positions_ssbo, 0, m_packed_chunk_positions.size() * sizeof(u32), m_packed_chunk_positions.data());	

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

void WorldRenderer::update_chunk(Chunk &chunk) {
	PROFILE_FUNC();
    
	const ChunkPosition position = chunk.get_position();

	const auto it = m_chunk_meshes.find(position);
	if(it == m_chunk_meshes.end()) {
		m_chunk_meshes.emplace(position, ChunkMesh(position));
	}

	m_chunk_meshes.at(position).generate_and_upload(chunk, *this);
    chunk.set_dirty(false);
}

void WorldRenderer::remove_chunk(const Chunk &chunk) {
	PROFILE_FUNC();

	const ChunkPosition position = chunk.get_position();

	const auto it = m_chunk_meshes.find(position);
	if(it == m_chunk_meshes.end()) {
		return;
	}

	ChunkMesh &mesh = it->second;
	free_chunk_mesh(mesh.m_alloc);

    m_chunk_meshes.erase(it);
}

void WorldRenderer::upload_chunk_mesh(const ChunkMeshAllocation &alloc, std::span<const u32> vertices, std::span<const u32> indices) {
	PROFILE_FUNC();
    
    if(vertices.size() > alloc.vertex_alloc.size) [[unlikely]] {
        std::println("WorldRenderer::upload_chunk_mesh allocated {} vertices, trying to upload {}.", alloc.vertex_alloc.size, vertices.size());
        return;
    }
    
    if(indices.size() > alloc.index_alloc.size) [[unlikely]] {
        std::println("WorldRenderer::upload_chunk_mesh allocated {} indices, trying to upload {}.", alloc.index_alloc.size, indices.size());
        return;
    }
    
	const u64 vert_offset = alloc.vertex_alloc.offset * sizeof(u32);
	const u64 index_offset = alloc.index_alloc.offset * sizeof(u32);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, vert_offset, vertices.size_bytes(), vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, indices.size_bytes(), indices.data());
}

void WorldRenderer::render_chunk_mesh(const ChunkMesh &mesh) {
	PROFILE_FUNC();

	DrawElementsIndirectCommand cmd = {
		.index_count = mesh.m_index_count,
		.instance_count = 1,
		.first_index = mesh.m_alloc.index_alloc.offset,
		.base_vertex = static_cast<i32>(mesh.m_alloc.vertex_alloc.offset),
		.base_instance = static_cast<u32>(m_draw_commands.size()),
	};
	
	m_draw_commands.push_back(cmd);
	m_packed_chunk_positions.push_back(Packer::pack_chunk_position(mesh.get_position()));
}

std::optional<ChunkMeshAllocation> WorldRenderer::allocate_chunk_mesh(u32 vertex_count, u32 index_count) {
    PROFILE_FUNC();
    
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
    
    return ChunkMeshAllocation{
        .vertex_alloc = *vertex_alloc,
        .index_alloc = *index_alloc
    };
}

void WorldRenderer::free_chunk_mesh(const ChunkMeshAllocation &alloc) {
    if(alloc.vertex_alloc.size > 0) {
        m_vertex_allocator.free(alloc.vertex_alloc);
    }
    
    if(alloc.index_alloc.size > 0) {
        m_index_allocator.free(alloc.index_alloc);
    }
}
