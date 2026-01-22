#include "game.hpp"

#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/graphics/renderers/text_renderer.hpp"
#include "vox/tools/fps_counter.hpp"
#include "vox/core/engine.hpp"
#include "vox/core/input.hpp"
#include "vox/world/world.hpp"

Game::Game() 
: m_camera(vec3(0, 30.0f, 0), 80.0f), m_player(m_camera) {
}

Game::~Game() {
}

bool Game::init() {
	m_world_renderer.init();
	m_text_renderer.init();
	m_block_outline_renderer.init();
	m_crosshair.init();
    
    return true;
}

void Game::update(f32 delta_time) {
	PROFILE_FUNC();
    
    handle_input();

	m_player.update(m_world, delta_time);
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 camera_matrix = m_camera.get_matrix(aspect_ratio);

    // TODO: Clear it only if a chunk changes
	m_world_renderer.clear();

	m_world.render(m_world_renderer);
	m_world_renderer.render(camera_matrix);

	const std::optional<BlockPosition> &last_highlighted_block_position = m_player.get_last_highlighted_block_position();
	if(last_highlighted_block_position.has_value()) {
		m_block_outline_renderer.render(last_highlighted_block_position.value(), camera_matrix);
	}
}

void Game::render_ui() {
	PROFILE_FUNC();

	m_text_renderer.render_text("Vox Engine", vec2(0, 0), 16.0f);
	m_text_renderer.render_text(std::format("FPS: {}", FpsCounter::get_instance().get_frame_rate()), vec2(0, 16), 16.0f);

	const ivec2 window_size = Engine::get_instance().get_window().get_size();
	m_crosshair.render(window_size);

	if(m_world_renderer.m_use_wireframe) {
		m_text_renderer.render_text("[wireframe]", vec2(0, window_size.y - 16.0f), 16.0f);
	}

	if(m_player.m_fly_enabled) {
		m_text_renderer.render_text("[fly]", vec2(0, window_size.y - 32.0f), 16.0f);
	}
}

void Game::render_imgui() {
    m_world_imgui_tool.render(m_world, m_world_renderer);
    m_graphics_imgui_tool.render(m_world);
}

void Game::handle_input() {
    Input &input = Input::get_instance();
	if(input.is_key_just_pressed(GLFW_KEY_F1)) {
		m_world_renderer.m_use_wireframe ^= true;
	}

	if(input.is_key_just_pressed(GLFW_KEY_F2)) {
		m_player.m_fly_enabled ^= true;
	}

	if(input.is_key_just_pressed(GLFW_KEY_ESCAPE)) {
		if(input.get_mouse_mode() == GLFW_CURSOR_DISABLED) {
			input.set_mouse_mode(GLFW_CURSOR_NORMAL);
        } else {
			input.set_mouse_mode(GLFW_CURSOR_DISABLED);
        }
	}
}

void Game::handle_window_resize(ivec2 window_size) {
	PROFILE_FUNC();
	m_text_renderer.handle_window_resize(window_size);
}