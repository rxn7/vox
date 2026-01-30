#pragma once

#include "vox/client/graphics/backend/dynamic_mesh.hpp"
#include "vox/client/graphics/backend/shader.hpp"
#include "vox/common/world/aabb.hpp"

struct DebugVertex {
	constexpr DebugVertex(vec3 position, vec3 color) : position(position), color(color) {}

	inline static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));
	}

	vec3 position;
	vec3 color;
};

class DebugRenderer {
public:
	DebugRenderer();

	void render(const mat4 &camera_matrix);

	void draw_aabb(const AABB &aabb, vec3 color);
	void draw_line(vec3 start, vec3 end, vec3 color);

private:
	DynamicMesh<DebugVertex> m_mesh;
	Shader m_shader;
	mat4 m_camera_matrix;

	std::vector<DebugVertex> m_vertices;
	std::vector<u16> m_elements;
};
