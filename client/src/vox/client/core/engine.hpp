#pragma once

#include "vox/common/system/tick_loop.hpp"
#include "vox/client/core/i_game.hpp"
#include "vox/client/core/window.hpp"
#include "vox/client/tools/imgui/profiler_imgui_tool.hpp"

class Engine {
public:
	Engine();
	~Engine();

	bool init();
	void run_game(IGame *game);

	static Engine &get_instance() { 
		return *sp_instance; 
	}

	inline Window &get_window() { 
		return m_window; 
	}

	inline f32 get_delta_time() const { 
		return m_tick_loop.get_elapsed();
	}

private:
	bool init_glfw();
	bool init_opengl();
	bool init_imgui();
	bool init_networking();

	void tick();
	void update(f32 dt);
	void render(f64 alpha);

	static void error_callback_glfw(i32 error, const char *description);
	static void window_size_callback_glfw(GLFWwindow *window, i32 w, i32 h);
	static void key_event_callback_glfw(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);
	static void mouse_button_event_callback_glfw(GLFWwindow *window, i32 button, i32 action, i32 mods);
	static void scroll_callback_glfw(GLFWwindow *window, f64 x, f64 y);
	static void mouse_move_callback_glfw(GLFWwindow *window, f64 x, f64 y);

private:
	Window m_window;
	TickLoop m_tick_loop;

	IGame *mp_game;
	bool m_is_opengl_initialized = false;

#ifndef NDEBUG
	ProfilerImGuiTool m_profiler_imgui_tool;
#endif
	
	static Engine *sp_instance;
};
