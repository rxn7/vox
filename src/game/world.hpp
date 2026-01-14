#pragma once

#include "core/camera.hpp"
#include "core/mesh.hpp"
#include "core/shader.hpp"

class World {
public:
	World();
	~World();

	void update(f32 delta_time);
	void render(f32 aspect_ratio);

private:
	std::unique_ptr<Camera> mp_camera;

	Shader m_shader;
	Mesh m_mesh;
};
