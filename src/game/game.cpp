#include "game.hpp"
#include "core/render/chunk_renderer.hpp"
#include "core/render/text_renderer.hpp"

Game::Game() 
: m_camera(vec3(0, 0, -5.0f), 80.0f) { }

Game::~Game() {
}

void Game::update(f32 delta_time) {
	PROFILE_FUNC();
	m_world.update();
	m_camera.update(delta_time);
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 camera_matrix = m_camera.get_matrix(aspect_ratio);
	ChunkRenderer::get_instance().render(camera_matrix);
}

void Game::render_ui() {
	PROFILE_FUNC();

	TextRenderer &text_renderer = TextRenderer::get_instance();
	text_renderer.render_text("Hello, world!", vec2(0, 0), 16.0f);
}
