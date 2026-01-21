#pragma once

#include "vox/graphics/resources/texture_id.hpp"
#include "vox/world/face_id.hpp"

namespace Packer {
	static constexpr u32 pack_chunk_position(i16vec3 p) {
		return static_cast<u32>(p.x & 0x3FFF) | static_cast<u32>((p.z & 0x3FFF) << 14) | static_cast<u32>((p.y & 0xF) << 28);
	}

	static constexpr u32 pack_vertex(u8 x, u8 y, u8 z, TextureID texture_id, FaceID face_id, u8 ambient_occlusion) {
		// 5 bits each for x, y, z (0-31)
		return (x & 31) 
			 | ((y & 31) << 5) 
			 | ((z & 31) << 10) 
			 | ((static_cast<u16>(texture_id) & 0x1FF) << 15) // 9 bits for texture (0-511)
			 | ((static_cast<u8>(face_id) & 0x7) << 24) // 3 bits for normal (0-5)
             | ((ambient_occlusion & 0x3) << 27) // 2 bits for ao
        ;
	}
};
