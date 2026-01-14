#pragma once

#include "game/world.hpp"

class App {
public:
	App();
	~App();
	void run();

private:
	bool init();
	bool init_glfw();
	bool init_opengl();

	static void error_callback_glfw(i32 error, const char *description);
	static void window_size_callback_glfw(GLFWwindow *window, i32 w, i32 h);
	static void key_event_callback_glfw(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);

private:
	static App *sp_instance;

	GLFWwindow *mp_window = nullptr;
	ivec2 m_window_size;

	std::unique_ptr<World> mp_world;
};
