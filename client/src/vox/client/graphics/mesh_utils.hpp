#pragma once

#include "vox/client/graphics/backend/vertex.hpp"

namespace MeshUtils {
	void add_face(
		std::vector<Default3DVertex> &vertices, 
		std::vector<u16> &indices, 
		vec3 center, 
		vec3 right, 
		vec3 up, 
		vec3 normal, 
		vec2 size, 
		vec2 uv_min, 
		vec2 uv_max
	);

	void create_box(std::vector<Default3DVertex> &vertices, std::vector<u16> &indices, vec3 size, vec2 uv_min = {0,0}, vec2 uv_max = {1,1});
}
