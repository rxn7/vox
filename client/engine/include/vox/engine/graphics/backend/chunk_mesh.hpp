#pragma once

#include "vox/common/world/chunk.hpp"
#include "vox/engine/core/allocators/offset_allocator.hpp"

class WorldRenderer;

struct ChunkMeshAllocation {
    OffsetAllocator::Allocation m_vertex_alloc;
    OffsetAllocator::Allocation m_index_alloc;
    
    inline bool is_valid() const {
        return m_vertex_alloc.m_size > 0 && m_index_alloc.m_size > 0;
    };
};

class ChunkMesh {
public:
    ChunkMesh(ChunkPosition position) : m_position(position) {}
    void generate_and_upload(const Chunk &chunk, WorldRenderer &renderer);

    inline ChunkPosition get_position() const {
        return m_position;
    }

public:
	u32 m_index_count = 0;
    ChunkMeshAllocation m_alloc;
    
private:
    ChunkPosition m_position;
};
