#pragma once

#include "vox/graphics/face_id.hpp"

struct FaceData {
	i8vec3 normal;
	i8vec3 tangent1;
	i8vec3 tangent2;
};

constexpr FaceData get_face_data(FaceID face_id) {
	switch(face_id) {
		case FaceID::Right:		return {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}};
		case FaceID::Left:		return {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}};
		case FaceID::Top:		return {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}};
		case FaceID::Bottom:	return {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}};
		case FaceID::Front:		return {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}};
		case FaceID::Back:		return {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}};
	}
}
