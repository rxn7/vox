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

		return	(static_cast<u32>(x) & 0x1F) |
				((static_cast<u32>(y) & 0x1F) << 5) |
			   	((static_cast<u32>(z) & 0x1F) << 10) |
			   	((static_cast<u32>(texture_id) & 0xFF) << 15) | 
			   	((static_cast<u32>(face_id) & 0x7) << 23) |
			   	((static_cast<u32>(ambient_occlusion) & 0x3) << 26);}
};
