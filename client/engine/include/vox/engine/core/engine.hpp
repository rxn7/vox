#pragma once

#include "vox/engine/core/i_game.hpp"
#include "vox/engine/core/window.hpp"
#include "vox/engine/tools/imgui/profiler_imgui_tool.hpp"

constexpr f32 PROFILING_DURATION_US = 3'000'000;

class Engine {
public:
	Engine();
	~Engine();

	void run_game(IGame *game);

	static Engine &get_instance() { 
        return *sp_instance; 
    }

    inline Window &get_window() { 
        return m_window; 
    }

	inline f32 get_delta_time() const { 
        return m_delta_time; 
    }

private:
	bool init();
	bool init_glfw();
	bool init_opengl();
	bool init_imgui();

	void update();
	void render();

	static void error_callback_glfw(i32 error, const char *description);
	static void window_size_callback_glfw(GLFWwindow *window, i32 w, i32 h);
	static void key_event_callback_glfw(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);
	static void mouse_button_event_callback_glfw(GLFWwindow *window, i32 button, i32 action, i32 mods);
	static void mouse_move_callback_glfw(GLFWwindow *window, f64 x, f64 y);

private:
    Window m_window;
	f32 m_delta_time;

    IGame *mp_game;
    
    ProfilerImGuiTool m_profiler_imgui_tool;
    
	static Engine *sp_instance;
};
