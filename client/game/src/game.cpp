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
			chunk.remove_subchunk(subchunk->m_idx);
		}
    });
}

Game::~Game() {
    m_world.m_chunk_removed_signal.disconnect(m_chunk_removed_callback);
}

bool Game::init() {
	PROFILE_FUNC();

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
					if(subchunk->is_empty()) {
						m_world_renderer.remove_subchunk(*subchunk);
						chunk.remove_subchunk(subchunk->m_idx);
						continue;
					}

					m_world_renderer.update_subchunk(*subchunk);
				}
			}
        }
    }
}

void Game::render_3d(f32 aspect_ratio) {
	PROFILE_FUNC();

	const mat4 view_matrix = m_camera.get_view_matrix();
	const mat4 proj_matrix = m_camera.get_projection_matrix(aspect_ratio);
	m_text_renderer.update_3d(proj_matrix, view_matrix);

	const mat4 camera_matrix = proj_matrix * view_matrix;
	m_world_renderer.render(camera_matrix);

	const std::optional<BlockPosition> &last_highlighted_block_position = m_player.get_last_highlighted_block_position();
	if(last_highlighted_block_position.has_value()) {
		m_block_outline_renderer.render(last_highlighted_block_position.value(), camera_matrix);
	}

	if(m_render_subchunk_debug) {
		for(auto &[position, chunk] : m_world.get_chunks()) {
			u32 idx = 0;
			for(const auto &subchunk : chunk.get_subchunks()) {
				const vec3 global_position = vec3(position.x, idx, position.y) * SUBCHUNK_SIZE;
				if(subchunk == nullptr) {
					++idx;
					continue;
				}

				++idx;

				TextRenderCommand3D cmd;
				cmd.text = std::format("{} {} {}", global_position.x, global_position.y, global_position.z);
				cmd.position = global_position;
				cmd.size = 1.0f;
				cmd.horizontal_align = TextHorizontalAlign::Center;
				cmd.vertical_align = TextVerticalAlign::Middle;
				cmd.billboard = true;

				const float t = global_position.y / static_cast<f32>(CHUNK_HEIGHT);
				cmd.color = vec4(
					1.0f,
					glm::min(1.0f, (1.0f - t) * 2.0f),
					glm::max(0.0f, 1.0f - t * 2.0f),
					1.0f
				);

				m_text_renderer.render_text_3d(cmd);
			}
		}
	}
}

void Game::render_ui() {
	PROFILE_FUNC();
	m_text_renderer.update_2d(Engine::get_instance().get_window().get_size());

	{
		TextRenderCommand2D cmd;
		cmd.text = "Vox Engine";
		cmd.position = vec2(0, 0);
		cmd.size = 16.0f;

		m_text_renderer.render_text_2d(cmd);
	}

	{
		TextRenderCommand2D cmd;
		cmd.text = std::format("fps: {}", FpsCounter::get_instance().get_frame_rate());
		cmd.position = vec2(0, 16);
		cmd.size = 16.0f;

		m_text_renderer.render_text_2d(cmd);
	}

	const ivec2 window_size = Engine::get_instance().get_window().get_size();
	m_crosshair.render(window_size);

	if(m_world_renderer.m_use_wireframe) {
		TextRenderCommand2D cmd;
		cmd.text = "[wireframe]";
		cmd.position = vec2(0, window_size.y - 16.0f);
		cmd.size = 16.0f;

		m_text_renderer.render_text_2d(cmd);
	}

	if(m_player.m_fly_enabled) {
		TextRenderCommand2D cmd;
		cmd.text = "[fly]";
		cmd.position = vec2(0, window_size.y - 32.0f);
		cmd.size = 16.0f;

		m_text_renderer.render_text_2d(cmd);
	}
}

void Game::render_imgui() {
	PROFILE_FUNC();
    m_world_imgui_tool.render(m_world, m_world_renderer);
    m_graphics_imgui_tool.render(m_world);
}

void Game::handle_input() {
	PROFILE_FUNC();

    Input &input = Input::get_instance();
	if(input.is_key_just_pressed(GLFW_KEY_F1)) {
		m_world_renderer.m_use_wireframe ^= 1;
	}

	if(input.is_key_just_pressed(GLFW_KEY_F2)) {
		m_player.m_fly_enabled ^= 1;
	}

	if(input.is_key_just_pressed(GLFW_KEY_F3)) {
		m_render_subchunk_debug ^= 1;
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
}
