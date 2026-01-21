#pragma once

#include "vox/core/allocators/offset_allocator.hpp"
#include "vox/graphics/backend/shader.hpp"
#include "vox/graphics/backend/texture_array.hpp"

struct ChunkMeshAllocation {
    OffsetAllocator::Allocation vertex_alloc;
    OffsetAllocator::Allocation index_alloc;
    
    inline bool is_valid() const {
        return vertex_alloc.size > 0 && index_alloc.size > 0;
    };
};

struct DrawElementsIndirectCommand {
	u32 index_count;
	u32 instance_count;
	u32 first_index;
	i32 base_vertex;
	u32 base_instance;
};

class WorldRenderer {
public:
	void init();
    
    void clear();
	void render(const mat4 &camera_matrix);

	void upload_chunk_mesh(const ChunkMeshAllocation &alloc, std::span<const u32> vertices, std::span<const u32> indices);
	void add_draw_command(const ChunkMeshAllocation &alloc, u32 index_count, i16vec3 chunk_position);
    
    [[nodiscard]] std::optional<ChunkMeshAllocation> allocate_chunk_mesh(u32 vertex_count, u32 index_count);
    void free_chunk_mesh(const ChunkMeshAllocation &alloc);
    
    inline const OffsetAllocator &get_vertex_allocator() const { return m_vertex_allocator; }
    inline const OffsetAllocator &get_index_allocator() const { return m_index_allocator; }
	
public:
	bool m_use_wireframe = false;

private:
	static constexpr u32 MAX_VERTICES = 20'000'000;
	static constexpr u32 MAX_INDICES = 30'000'000;
    static constexpr u32 MAX_VISIBLE_CHUNKS = 64;

	u32 m_vao;
	u32 m_vbo;
	u32 m_ebo;

	u32 m_indirect_buffer;
	u32 m_packed_chunk_positions_ssbo;

	Shader m_shader;
	TextureArray m_textures;

	std::vector<DrawElementsIndirectCommand> m_draw_commands;
	std::vector<u32> m_packed_chunk_positions;
    
    OffsetAllocator m_vertex_allocator{MAX_VERTICES};
    OffsetAllocator m_index_allocator{MAX_INDICES};
};
