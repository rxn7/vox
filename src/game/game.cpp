#include "game/game.hpp"
#include "graphics/renderers/chunk_renderer.hpp"
#include "graphics/renderers/text_renderer.hpp"
#include "core/input.hpp"

Game::Game() 
: m_camera(vec3(0, 30.0f, 0), 80.0f) { 
	m_chunk_renderer.init();
	m_text_renderer.init();
}

Game::~Game() {
}

void Game::update(f32 delta_time) {
	PROFILE_FUNC();

	m_camera.update(delta_time);
	m_chunk_renderer.new_frame();

	const Input &input = Input::get_instance();
	if(input.is_key_just_pressed(GLFW_KEY_F1)) {
		m_chunk_renderer.wireframe ^= true;
	}
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 camera_matrix = m_camera.get_matrix(aspect_ratio);
	m_world.render(m_chunk_renderer);
	m_chunk_renderer.render(camera_matrix);
}

void Game::render_ui() {
	PROFILE_FUNC();

	m_text_renderer.render_text("Hello, world!", vec2(0, 0), 16.0f);
}

void Game::handle_window_resize(vec2 window_size) {
	PROFILE_FUNC();
	m_text_renderer.handle_window_resize(window_size);
}
