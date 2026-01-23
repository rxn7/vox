#pragma once

#include "player/player.hpp"
#include "ui/crosshair.hpp"

#include "vox/common/world/world.hpp"
#include "vox/engine/core/i_game.hpp"
#include "vox/engine/tools/imgui/graphics_imgui_tool.hpp"
#include "vox/engine/tools/imgui/world_imgui_tool.hpp"
#include "vox/engine/graphics/backend/camera.hpp"
#include "vox/engine/graphics/renderers/block_outline_renderer.hpp"
#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/engine/graphics/renderers/text_renderer.hpp"

class Game : public IGame {
public:
	Game();
	~Game();
    
    bool init() override;

	void update(f32 delta_time) override;
	void render_3d(f32 aspect_ratio) override;
	void render_ui() override;
	void render_imgui() override;
	void handle_window_resize(ivec2 window_size) override;

	inline const Camera &get_camera() const { 
        return m_camera; 
    }
    
private:
    void handle_input();

private:
	Player m_player;
	World m_world;

	WorldRenderer m_world_renderer;
	TextRenderer m_text_renderer;
	BlockOutlineRenderer m_block_outline_renderer;

	Crosshair m_crosshair;
	Camera m_camera;
    
    WorldImGuiTool m_world_imgui_tool;
    GraphicsImGuiTool m_graphics_imgui_tool;
};
