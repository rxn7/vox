#include "vox/core/allocators/offset_allocator.hpp"
#include "vox/graphics/backend/packer.hpp"
#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/graphics/resources/texture_paths.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

#include <battery/embed.hpp>
#include <optional>

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

void WorldRenderer::clear() {
	m_draw_commands.clear();
	m_packed_chunk_positions.clear();
}

void WorldRenderer::render(const mat4 &camera_matrix) {
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

	if(m_use_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} 

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, m_draw_commands.size(), 0);

	if(m_use_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
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

void WorldRenderer::add_draw_command(const ChunkMeshAllocation &alloc, u32 index_count, i16vec3 chunk_position) {
	PROFILE_FUNC();

	DrawElementsIndirectCommand cmd = {
		.index_count = alloc.index_alloc.size,
		.instance_count = 1,
		.first_index = alloc.index_alloc.offset,
		.base_vertex = static_cast<i32>(alloc.vertex_alloc.offset),
		.base_instance = static_cast<u32>(m_draw_commands.size()),
	};
	
	m_draw_commands.push_back(cmd);
	m_packed_chunk_positions.push_back(Packer::pack_chunk_position(chunk_position));
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