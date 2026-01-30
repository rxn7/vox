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
	
	WorldImGuiTool m_world_imgui_tool;
	GraphicsImGuiTool m_graphics_imgui_tool;

	bool m_render_subchunk_debug = false;

	CallbackID m_chunk_removed_callback;
};
