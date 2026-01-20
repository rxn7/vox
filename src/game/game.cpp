#include "game/game.hpp"
#include "graphics/renderers/chunk_renderer.hpp"
#include "graphics/renderers/text_renderer.hpp"
#include "tools/fps_counter.hpp"
#include "core/app.hpp"
#include "core/input.hpp"

Game::Game() 
: m_camera(vec3(0, 30.0f, 0), 80.0f), m_player(m_camera) {
	m_chunk_renderer.init();
	m_text_renderer.init();
	m_crosshair.init();
}

Game::~Game() {
}

void Game::update(f32 delta_time) {
	PROFILE_FUNC();

	Input &input = Input::get_instance();
	if(input.is_key_just_pressed(GLFW_KEY_F1)) {
		m_chunk_renderer.m_use_wireframe ^= true;
	}
	if(input.is_key_just_pressed(GLFW_KEY_F2)) {
		m_player.m_fly_enabled ^= true;
	}
	if(input.is_key_just_pressed(GLFW_KEY_ESCAPE)) {
		if(input.get_mouse_mode() == GLFW_CURSOR_DISABLED)
			input.set_mouse_mode(GLFW_CURSOR_NORMAL);
		else
			input.set_mouse_mode(GLFW_CURSOR_DISABLED);
	}

	m_player.update(m_world, delta_time);
	m_chunk_renderer.new_frame();
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 camera_matrix = m_camera.get_matrix(aspect_ratio);
	m_world.render(m_chunk_renderer);
	m_chunk_renderer.render(camera_matrix);
}

void Game::render_ui() {
	PROFILE_FUNC();

	m_text_renderer.render_text("Vox Engine", vec2(0, 0), 16.0f);
	m_text_renderer.render_text(std::format("FPS: {}", FpsCounter::get_instance().get_frame_rate()), vec2(0, 16), 16.0f);

	const vec2 window_size = App::get_instance().get_window_size();
	m_crosshair.render(window_size);

	if(m_chunk_renderer.m_use_wireframe) {
		m_text_renderer.render_text("[wireframe]", vec2(0, window_size.y - 16.0f), 16.0f);
	}

	if(m_player.m_fly_enabled) {
		m_text_renderer.render_text("[fly]", vec2(0, window_size.y - 32.0f), 16.0f);
	}
}

void Game::handle_window_resize(vec2 window_size) {
	PROFILE_FUNC();
	m_text_renderer.handle_window_resize(window_size);
}
