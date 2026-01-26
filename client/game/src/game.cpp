#include "game.hpp"

#include "vox/common/world/subchunk.hpp"
#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/engine/graphics/renderers/text_renderer.hpp"
#include "vox/engine/tools/fps_counter.hpp"
#include "vox/engine/core/engine.hpp"
#include "vox/engine/core/input.hpp"
#include "vox/common/world/world.hpp"

Game::Game() 
: m_camera(vec3(0, 120.0f, 0), 75.0f), m_player(m_camera) {
    m_chunk_removed_callback = m_world.m_chunk_removed_signal.connect([&](Chunk &chunk) {
		for(const auto &subchunk : chunk.get_subchunks()) {
			if(subchunk == nullptr) {
				continue;	
			}
			m_world_renderer.remove_subchunk(*subchunk);
		}
    });
}

Game::~Game() {
    m_world.m_chunk_removed_signal.disconnect(m_chunk_removed_callback);
}

bool Game::init() {
	m_world_renderer.init();
	m_text_renderer.init();
	m_block_outline_renderer.init();
	m_crosshair.init();

    m_world.create_initial_chunks();

    return true;
}

void Game::update(f32 delta_time) {
	PROFILE_FUNC();
    
    handle_input();
	m_player.update(m_world, delta_time);
    
    for(auto &[position, chunk] : m_world.get_chunks()) {
        if(chunk.has_dirty_subchunks()) {
			for(const auto &subchunk : chunk.get_subchunks()) {
				if(subchunk == nullptr) {
					continue;
				}

				if(chunk.is_dirty(subchunk->m_idx)) {
					m_world_renderer.update_subchunk(*subchunk);
				}
			}
        }
    }
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 camera_matrix = m_camera.get_matrix(aspect_ratio);

	m_world_renderer.render(camera_matrix);

	const std::optional<BlockPosition> &last_highlighted_block_position = m_player.get_last_highlighted_block_position();
	if(last_highlighted_block_position.has_value()) {
		m_block_outline_renderer.render(last_highlighted_block_position.value(), camera_matrix);
	}
}

void Game::render_ui() {
	PROFILE_FUNC();

	m_text_renderer.render_text("Vox Engine", vec2(0, 0), 16.0f);
	m_text_renderer.render_text(std::format("fps: {}", FpsCounter::get_instance().get_frame_rate()), vec2(0, 16), 16.0f);
	m_text_renderer.render_text(std::format("pos: {}", m_camera.m_position), vec2(0, 32), 16.0f);

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
