#include "core/input.hpp"

void Input::new_frame() {
	m_last_mouse_position = m_mouse_position;
	m_previous_keys = m_keys;
}

bool Input::is_key_pressed(i32 key) const {
	const auto it = m_keys.find(key);

	if(it == m_keys.end())
		return false;

	return it->second;
}

bool Input::is_key_just_pressed(i32 key) const {
	const auto it = m_keys.find(key);

	if(it == m_keys.end())
		return false;

	const auto previous_it = m_previous_keys.find(key);
	if(previous_it != m_previous_keys.end())
		return false;

	return true;
}

void Input::set_mouse_mode(GLFWwindow *p_window, i32 mode) {
	if(m_mouse_mode == mode) {
		return;
	}

	m_mouse_mode = mode;
	glfwSetInputMode(p_window, GLFW_CURSOR, mode);
}

void Input::update_mouse_position(vec2 position) {
	m_mouse_position = position;
}

void Input::update_key(GLFWwindow *p_window, i32 key, bool is_pressed) {
	switch(key) {
		case GLFW_KEY_ESCAPE: {
			if(!is_pressed)
				return;

			if(get_mouse_mode() == GLFW_CURSOR_DISABLED)
				set_mouse_mode(p_window, GLFW_CURSOR_NORMAL);
			else
				set_mouse_mode(p_window, GLFW_CURSOR_DISABLED);

			break;
		}

		default: m_keys[key] = is_pressed;
	}
}
