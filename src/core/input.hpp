#pragma once

#include "singleton.hpp"

struct InputState {
	bool move_left = false;
	bool move_right = false;
	bool move_forward = false;
	bool move_backward = false;

	bool jump = false;
	bool crouch = false;

	bool toggle_wireframe = false;

	vec2 mouse_delta = vec2(0);
};

class InputManager {
SINGLETON_CLASS(InputManager);
public:
	InputManager() {}

	void set_mouse_mode(GLFWwindow *p_window, i32 mode);
	void update_mouse_position(vec2 position);
	void handle_key_event(GLFWwindow *p_window, i32 key, bool is_pressed);
	[[nodiscard]] inline i32 get_mouse_mode() { return m_mouse_mode; }
	[[nodiscard]] inline const InputState &get_state() { return m_state; }

private:
	std::optional<vec2> m_last_mouse_position = std::nullopt;
	i32 m_mouse_mode = GLFW_CURSOR_NORMAL;
	InputState m_state = {0};
};
