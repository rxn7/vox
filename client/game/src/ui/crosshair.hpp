#pragma once

#include "vox/engine/graphics/backend/mesh.hpp"
#include "vox/engine/graphics/backend/shader.hpp"

struct CrosshairVertex {
	static inline void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CrosshairVertex), (void*)offsetof(CrosshairVertex, m_uv));
	}

	vec2 m_uv;
};


class Crosshair {
public:
	Crosshair();
	~Crosshair();

	void init();
	void render(vec2 window_size);

public:
	float m_size = 24.0f;
	float m_thickness = 0.1f;

private:
	Mesh<CrosshairVertex> m_mesh;
	Shader m_shader;
};
