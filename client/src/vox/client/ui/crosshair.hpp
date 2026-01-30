#pragma once

#include "vox/client/graphics/backend/static_mesh.hpp"
#include "vox/client/graphics/backend/shader.hpp"

struct CrosshairVertex {
	static inline void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CrosshairVertex), (void*)offsetof(CrosshairVertex, uv));
	}

	vec2 uv;
};


class Crosshair {
public:
	Crosshair();
	~Crosshair();

	void render(vec2 window_size);

public:
	float m_size = 24.0f;
	float m_thickness = 0.1f;

private:
	StaticMesh<CrosshairVertex> m_mesh;
	Shader m_shader;
};
