#pragma once

#include "core/singleton.hpp"

class Input {
SINGLETON_CLASS(Input);
public:
	void init(GLFWwindow *p_window);

	// this has to be called right before glfwPollEvents
	void new_frame();

	bool is_key_pressed(i32 key) const;
	bool is_key_just_pressed(i32 key) const;

	bool is_mouse_button_pressed(i32 button) const;
	bool is_mouse_button_just_pressed(i32 button) const;

	void set_mouse_mode(i32 mode);

	void update_mouse_position(vec2 position);
	void update_key(i32 key, bool is_pressed);
	void update_mouse_button(i32 button, bool is_pressed);

	inline vec2 get_mouse_delta() const { return m_mouse_position - m_last_mouse_position.value_or(m_mouse_position); }
	inline i32 get_mouse_mode() const { return m_mouse_mode; }

private:
	std::array<bool, GLFW_KEY_LAST+1> m_keys;
	std::array<bool, GLFW_KEY_LAST+1> m_previous_keys;

	std::array<bool, GLFW_MOUSE_BUTTON_LAST+1> m_buttons;
	std::array<bool, GLFW_MOUSE_BUTTON_LAST+1> m_previous_buttons;

	std::optional<vec2> m_last_mouse_position = std::nullopt;
	vec2 m_mouse_position = vec2(0);

	i32 m_mouse_mode = GLFW_CURSOR_NORMAL;

	GLFWwindow *mp_window = nullptr;
};
