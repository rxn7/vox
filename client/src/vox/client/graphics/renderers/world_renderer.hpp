#pragma once

#include "vox/client/core/allocators/offset_allocator.hpp"
#include "vox/client/graphics/backend/subchunk_mesh.hpp"
#include "vox/client/graphics/backend/shader.hpp"
#include "vox/client/graphics/backend/texture_array.hpp"

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
	void render(const mat4 &camera_matrix);

	void update_subchunk(SubChunk &subchunk);
	void remove_subchunk(SubChunk &subchunk);

	void upload_subchunk_mesh(const SubChunkMeshAllocation &alloc, std::span<const u32> vertices, std::span<const u32> indices);
	[[nodiscard]] std::optional<SubChunkMeshAllocation> allocate_subchunk_mesh(u32 vertex_count, u32 index_count);
	void free_subchunk_mesh(const SubChunkMeshAllocation &alloc);
	
	inline const OffsetAllocator &get_vertex_allocator() const {
		return m_vertex_allocator; 
	}

	inline const OffsetAllocator &get_index_allocator() const {
		return m_index_allocator; 
	}
	
private:
	void render_subchunk_mesh(const SubChunkMesh &mesh);
	
public:
	static constexpr u32 MAX_VERTICES = 20'000'000;
	static constexpr u32 MAX_INDICES = 30'000'000;
	static constexpr u32 MAX_VISIBLE_SUBCHUNKS = 10'000;

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
	std::vector<u32> m_packed_subchunk_positions;
	std::unordered_map<SubChunkPosition, SubChunkMesh> m_subchunk_meshes;
	
	OffsetAllocator m_vertex_allocator{MAX_VERTICES};
	OffsetAllocator m_index_allocator{MAX_INDICES};
};
