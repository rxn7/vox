#pragma once

#include "tools/imgui/profiler_window.hpp"
#include "game/game.hpp"

class App {
public:
	App();
	~App();
	void run();

private:
	bool init();
	bool init_glfw();
	bool init_opengl();
	bool init_imgui();

	void update();
	void render();
	void render_3d();
	void render_ui();
	void render_imgui();

	static void error_callback_glfw(i32 error, const char *description);
	static void window_size_callback_glfw(GLFWwindow *window, i32 w, i32 h);
	static void key_event_callback_glfw(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);
	static void mouse_move_callback_glfw(GLFWwindow *window, f64 x, f64 y);

private:
	GLFWwindow *mp_window = nullptr;
	ivec2 m_window_size;
	f32 m_delta_time;

	ProfilerWindow m_profiler_window;
	std::unique_ptr<Game> mp_game;

	static App *sp_instance;
};
