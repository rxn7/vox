#pragma once

#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/world/chunk.hpp"

namespace ChunkMeshGenerator {
    // return the number of indices uploaded
	u32 generate_mesh_and_upload(const Chunk &chunk, ChunkMeshAllocation &alloc, WorldRenderer &renderer);

	inline u8 calculate_vertex_ao(bool side1_occluded, bool side2_occluded, bool corner_occluded) {
        if(side1_occluded && side2_occluded) {
            return 3;
        }
        return side1_occluded + side2_occluded + corner_occluded;
    }
}