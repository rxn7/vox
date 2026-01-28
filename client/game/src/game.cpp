#include "game.hpp"

#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/engine/graphics/renderers/text_renderer.hpp"
#include "vox/engine/tools/fps_counter.hpp"
#include "vox/engine/core/engine.hpp"
#include "vox/engine/core/input.hpp"
#include "vox/common/world/world.hpp"
#include "vox/common/world/block_registry.hpp"
#include "vox/common/world/subchunk.hpp"

Game::Game() 
: mp_host_driver(std::make_shared<HostNetworkDriver>()),
m_server(mp_host_driver), 
m_camera(vec3(0.0f, 120.0f, 0.0f)), 
m_player(m_camera) {
	m_chunk_removed_callback = m_world.m_chunk_removed_signal.connect([&](Chunk &chunk) {
		for(const auto &subchunk : chunk.get_subchunks()) {
			if(subchunk == nullptr) {
				continue;	
			}

			m_world_renderer.remove_subchunk(*subchunk);
			chunk.remove_subchunk(subchunk->get_idx());
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

	m_server_thread = std::jthread([this](std::stop_token stop_token) {
		m_server.run(stop_token);
	});

	return true;
}

void Game::tick() {
	PROFILE_FUNC();

	// C2S_ChatMessagePacket chat_packet = {
	// 	.message = "Hello world!"
	// };
	// mp_host_driver->SendPacketToServer(std::move(chat_packet));

	m_player.tick(m_world);
}

void Game::update(f64 alpha, f32 delta_time) {
	PROFILE_FUNC();
	
	handle_input();
	m_player.update(alpha);
	
	for(auto &[position, chunk] : m_world.get_chunks()) {
		if(chunk.has_dirty_subchunks()) {
			for(const auto &subchunk : chunk.get_subchunks()) {
				if(subchunk == nullptr) {
					continue;
				}

				if(chunk.is_dirty(subchunk->get_idx())) {
					if(subchunk->is_empty()) {
						m_world_renderer.remove_subchunk(*subchunk);
						chunk.remove_subchunk(subchunk->get_idx());
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
			for(const auto &subchunk : chunk.get_subchunks()) {
				if(subchunk == nullptr) {
					continue;
				}


				TextRenderCommand3D cmd;
				cmd.text = std::format("{} {} {}", position.x, subchunk->get_idx(), position.y);
				cmd.position = vec3(position.x, subchunk->get_idx(), position.y) * SUBCHUNK_SIZE;
				cmd.size = 0.5f;
				cmd.horizontal_align = TextHorizontalAlign::Center;
				cmd.vertical_align = TextVerticalAlign::Middle;
				cmd.billboard = true;

				const float t = subchunk->get_idx() / static_cast<f32>(SUBCHUNK_COUNT);
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

	const ivec2 window_size = Engine::get_instance().get_window().get_size();

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

	{
		TextRenderCommand2D cmd;
		cmd.text = std::format("pos: {:.2f} {:.2f} {:.2f}", m_player.get_position().x, m_player.get_position().y, m_player.get_position().z);
		cmd.position = vec2(0, 32);
		cmd.size = 16.0f;

		m_text_renderer.render_text_2d(cmd);
	}

	if(m_world_renderer.m_use_wireframe) {
		TextRenderCommand2D cmd;
		cmd.text = "[wireframe]";
		cmd.position = vec2(0, window_size.y);
		cmd.size = 16.0f;
		cmd.vertical_align = TextVerticalAlign::Top;

		m_text_renderer.render_text_2d(cmd);
	}

	if(m_player.m_fly_enabled) {
		TextRenderCommand2D cmd;
		cmd.text = "[fly]";
		cmd.position = vec2(0, window_size.y - 16.0f);
		cmd.size = 16.0f;
		cmd.vertical_align = TextVerticalAlign::Top;

		m_text_renderer.render_text_2d(cmd);
	}

	{
		TextRenderCommand2D cmd;
		cmd.text = std::format("block: {}", BlockRegistry::get(m_player.get_block_in_hand()).get_name());
		cmd.position = vec2(window_size.x * 0.5f, 16.0f);
		cmd.size = 24.0f;
		cmd.horizontal_align = TextHorizontalAlign::Center;
		cmd.vertical_align = TextVerticalAlign::Middle;
		cmd.color = vec4(1.0f, 1.0f, 1.0f, 0.7f);

		m_text_renderer.render_text_2d(cmd);
	}

	m_crosshair.render(window_size);
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
