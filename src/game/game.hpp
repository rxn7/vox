#pragma once

#include "core/camera.hpp"
#include "world/world.hpp"
#include "graphics/renderers/chunk_renderer.hpp"
#include "graphics/renderers/text_renderer.hpp"

class Game {
public:
	Game();
	~Game();

	void update(f32 delta_time);
	void render_3d(f32 aspect_ratio);
	void render_ui();
	void handle_window_resize(vec2 window_size);

	inline const Camera &get_camera() const { return m_camera; }

private:
	ChunkRenderer m_chunk_renderer;
	TextRenderer m_text_renderer;

	Camera m_camera;
	World m_world;
};
