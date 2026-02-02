#include "vox/client/graphics/mesh_utils.hpp"

void MeshUtils::add_face(std::vector<Default3DVertex> &vertices, std::vector<u16> &indices, vec3 center, vec3 right, vec3 up, vec3 normal, vec2 size, vec2 uv_min, vec2 uv_max) {
	const u16 start_idx = static_cast<u16>(vertices.size());

	const vec3 right_offset = right * (size.x * 0.5f);
	const vec3 up_offset = up * (size.y * 0.5f);

	vertices.emplace_back(Default3DVertex(center - right_offset - up_offset, normal, {uv_min.x, uv_min.y})); // 0: BL
	vertices.emplace_back(Default3DVertex(center + right_offset - up_offset, normal, {uv_max.x, uv_min.y})); // 1: BR
	vertices.emplace_back(Default3DVertex(center + right_offset + up_offset, normal, {uv_max.x, uv_max.y})); // 2: TR
	vertices.emplace_back(Default3DVertex(center - right_offset + up_offset, normal, {uv_min.x, uv_max.y})); // 3: TL

	indices.push_back(start_idx + 0);
	indices.push_back(start_idx + 1);
	indices.push_back(start_idx + 2);

	indices.push_back(start_idx + 2);
	indices.push_back(start_idx + 3);
	indices.push_back(start_idx + 0);
}

void MeshUtils::create_box(std::vector<Default3DVertex> &vertices, std::vector<u16> &indices, vec3 size, vec2 uv_min, vec2 uv_max) {
	const vec3 half_size = size * 0.5f;

	// front face (Z+)
	add_face(vertices, indices, vec3(0, 0, half_size.z), RIGHT, UP, FRONT, {size.x, size.y}, uv_min, uv_max);

	// back face (Z-)
	add_face(vertices, indices, vec3(0, 0, -half_size.z), -RIGHT, UP, BACK, {size.x, size.y}, uv_min, uv_max);

	// right face (X+)
	add_face(vertices, indices, vec3(half_size.x, 0, 0), -FRONT, UP, RIGHT, {size.z, size.y}, uv_min, uv_max);

	// left face (X-)
	add_face(vertices, indices, vec3(-half_size.x, 0, 0), FRONT, UP, LEFT, {size.z, size.y}, uv_min, uv_max);

	// top face (Y+)
	add_face(vertices, indices, vec3(0, half_size.y, 0), RIGHT, -FRONT, UP, {size.x, size.z}, uv_min, uv_max);

	// bottom face (Y-)
	add_face(vertices, indices, vec3(0, -half_size.y, 0), RIGHT, FRONT, DOWN, {size.x, size.z}, uv_min, uv_max);
}
