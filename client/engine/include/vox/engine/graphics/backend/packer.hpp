#pragma once

#include "vox/common/profiler/profiler_scope_timer.hpp"
#include "vox/common/resources/texture_id.hpp"
#include "vox/common/world/block_face_id.hpp"
#include "vox/common/world/chunk_position.hpp"

namespace Packer {
	static u32 pack_subchunk_position(SubChunkPosition p) {
        PROFILE_FUNC();

        u32 packed = 0;
        packed = glm::bitfieldInsert(packed, static_cast<u32>(p.x), 0, 12);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(p.y), 12, 8);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(p.z), 20, 12);
        return packed;
	}

	static u32 pack_vertex(u8 x, u8 y, u8 z, TextureID texture_id, BlockFaceID face_id, u8 ambient_occlusion) {
        PROFILE_FUNC();

        u32 packed = 0;
        packed = glm::bitfieldInsert(packed, static_cast<u32>(x), 0, 5);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(y), 5, 5);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(z), 10, 5);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(texture_id), 15, 8);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(face_id), 23, 3);
        packed = glm::bitfieldInsert(packed, static_cast<u32>(ambient_occlusion), 26, 2);
        return packed;
	}
};
