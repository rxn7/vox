#pragma once

#include "vox/common/world/block_face_id.hpp"

struct FaceData {
	i8vec3 normal;
	i8vec3 tangent1;
	i8vec3 tangent2;
};

constexpr FaceData get_face_data(BlockFaceID face_id) {
	switch(face_id) {
		case BlockFaceID::Right:		return {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}};
		case BlockFaceID::Left:		return {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}};
		case BlockFaceID::Top:		return {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}};
		case BlockFaceID::Bottom:	return {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}};
		case BlockFaceID::Front:		return {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}};
		case BlockFaceID::Back:		return {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}};
	}
}
