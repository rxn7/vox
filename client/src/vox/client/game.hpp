#pragma once

#include "player/player.hpp"
#include "ui/crosshair.hpp"

#include "vox/client/world/client_world.hpp"
#include "vox/common/helper/signal.hpp"
#include "vox/common/server/server_logic.hpp"
#include "vox/client/networking/drivers/host_network_driver.hpp"
#include "vox/client/core/i_game.hpp"
#include "vox/client/tools/imgui/graphics_imgui_tool.hpp"
#include "vox/client/tools/imgui/world_imgui_tool.hpp"
#include "vox/client/graphics/backend/camera.hpp"
#include "vox/client/graphics/renderers/block_outline_renderer.hpp"
#include "vox/client/graphics/renderers/world_renderer.hpp"
#include "vox/client/graphics/renderers/text_renderer.hpp"
#include "vox/client/graphics/renderers/debug_renderer.hpp"

class Game : public IGame {
public:
	Game();
	~Game();
	
	bool init() override;

	void tick() override;
	void update(f64 alpha, f32 delta_time) override;
	void render_3d(f32 aspect_ratio) override;
	void render_ui() override;
	void render_imgui() override;

	inline const Camera &get_camera() const { 
		return m_camera; 
	}

	inline DebugRenderer &get_debug_renderer() {
		return m_debug_renderer;
	}

	inline Shader &get_default_3d_shader() {
		return m_default_3d_shader;
	}

	inline bool is_debug_enabled() const {
		return m_debug_enabled;
	}

	inline static Game *get_instance() {
		return sp_game;
	}
	
private:
	void render_chunks_debug();
	void start_server();
	void handle_input();
	void handle_packet(S2C_Packet packet);

private:
	// TODO: this should be only inside host
	std::shared_ptr<HostNetworkDriver> mp_network;
	ServerLogic m_server;
	std::jthread m_server_thread;

	ClientWorld m_world;

	WorldRenderer m_world_renderer;
	TextRenderer m_text_renderer;
	BlockOutlineRenderer m_block_outline_renderer;
	DebugRenderer m_debug_renderer;

	Crosshair m_crosshair;
	Camera m_camera;
	Player m_player;

	Shader m_default_3d_shader;
	
	WorldImGuiTool m_world_imgui_tool;
	GraphicsImGuiTool m_graphics_imgui_tool;

	bool m_debug_enabled = false;

	CallbackID m_chunk_removed_callback;

	inline static Game *sp_game;
};
