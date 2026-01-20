#pragma once

#include "graphics/backend/mesh.hpp"
#include "graphics/backend/shader.hpp"

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
	Mesh m_mesh;
	Shader m_shader;
};
