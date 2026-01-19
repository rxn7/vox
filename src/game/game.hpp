#pragma once

#include "core/camera.hpp"
#include "core/world.hpp"

class Game {
public:
	Game();
	~Game();

	void update(f32 delta_time);
	void render_3d(f32 aspect_ratio);
	void render_ui();

	inline const Camera &get_camera() const { return m_camera; }

private:
	Camera m_camera;
	World m_world;
};
