#pragma once

#include "vox/common/world/chunk.hpp"
#include "vox/engine/core/allocators/offset_allocator.hpp"

class WorldRenderer;

struct SubChunkMeshAllocation {
	OffsetAllocator::Allocation vertex_alloc;
	OffsetAllocator::Allocation index_alloc;
	
	inline bool is_valid() const {
		return vertex_alloc.m_size > 0 && index_alloc.m_size > 0;
	};
};

class SubChunkMesh {
public:
	SubChunkMesh(SubChunkPosition position) : m_position(position) {}

	void generate_and_upload(const SubChunk &chunk, WorldRenderer &renderer);

	inline SubChunkPosition get_position() const {
		return m_position;
	}

public:
	u32 m_index_count = 0;
	SubChunkMeshAllocation m_alloc;
	
private:
	SubChunkPosition m_position;
};
