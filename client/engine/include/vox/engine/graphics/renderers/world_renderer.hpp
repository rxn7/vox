#pragma once

#include "vox/common/world/chunk.hpp"
#include "vox/engine/core/allocators/offset_allocator.hpp"
#include "vox/engine/graphics/backend/chunk_mesh.hpp"
#include "vox/engine/graphics/backend/shader.hpp"
#include "vox/engine/graphics/backend/texture_array.hpp"

struct DrawElementsIndirectCommand {
	u32 m_index_count;
	u32 m_instance_count;
	u32 m_first_index;
	i32 m_base_vertex;
	u32 m_base_instance;
};

class WorldRenderer {
public:
	void init();
	void render(const mat4 &camera_matrix);

	void update_chunk(Chunk &chunk);
	void remove_chunk(const Chunk &chunk);

	void upload_chunk_mesh(const ChunkMeshAllocation &alloc, std::span<const u32> vertices, std::span<const u32> indices);
    [[nodiscard]] std::optional<ChunkMeshAllocation> allocate_chunk_mesh(u32 vertex_count, u32 index_count);
    void free_chunk_mesh(const ChunkMeshAllocation &alloc);
    
    inline const OffsetAllocator &get_vertex_allocator() const {
        return m_vertex_allocator; 
    }

    inline const OffsetAllocator &get_index_allocator() const {
        return m_index_allocator; 
    }
    
private:
	void render_chunk_mesh(const ChunkMesh &mesh);
	
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
    std::unordered_map<ChunkPosition, ChunkMesh> m_chunk_meshes;
    
    OffsetAllocator m_vertex_allocator{MAX_VERTICES};
    OffsetAllocator m_index_allocator{MAX_INDICES};
};
