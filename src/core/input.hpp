#pragma once

#include "core/singleton.hpp"

class Input {
SINGLETON_CLASS(Input);
public:
	// this has to be called right before glfwPollEvents
	void new_frame();

	bool is_key_pressed(i32 key) const;
	bool is_key_just_pressed(i32 key) const;

	void set_mouse_mode(GLFWwindow *p_window, i32 mode);

	void update_mouse_position(vec2 position);
	void update_key(GLFWwindow *p_window, i32 key, bool is_pressed);

	inline vec2 get_mouse_delta() const { return m_mouse_position - m_last_mouse_position.value_or(m_mouse_position); }
	inline i32 get_mouse_mode() const { return m_mouse_mode; }

private:
	std::unordered_map<i32, bool> m_keys;
	std::unordered_map<i32, bool> m_previous_keys;

	std::optional<vec2> m_last_mouse_position = std::nullopt;
	vec2 m_mouse_position = vec2(0);

	i32 m_mouse_mode = GLFW_CURSOR_NORMAL;
};
