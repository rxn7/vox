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

	inline const Camera &get_camera() const { return m_camera; }

private:
	Camera m_camera;
	Shader m_shader;
	Mesh m_mesh;
};
