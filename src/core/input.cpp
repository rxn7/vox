#include "core/input.hpp"

void Input::init(GLFWwindow *p_window) {
	mp_window = p_window;
}

void Input::new_frame() {
	m_last_mouse_position = m_mouse_position;
	m_previous_keys = m_keys;
	m_previous_buttons = m_buttons;
}

bool Input::is_key_pressed(i32 key) const {
	if(key < 0 || key > GLFW_KEY_LAST) [[unlikely]] {
		return false;
	}

	return m_keys[key];
}

bool Input::is_key_just_pressed(i32 key) const {
	if(key < 0 || key > GLFW_KEY_LAST) [[unlikely]] {
		return false;
	}

	return m_keys[key] && !m_previous_keys[key];
}

bool Input::is_mouse_button_pressed(i32 button) const {
	if(button < 0 || button > GLFW_MOUSE_BUTTON_LAST) [[unlikely]] {
		return false;
	}

	return m_buttons[button];
}

bool Input::is_mouse_button_just_pressed(i32 button) const {
	if(button < 0 || button > GLFW_MOUSE_BUTTON_LAST) [[unlikely]] {
		return false;
	}

	return m_buttons[button] && !m_previous_buttons[button];
}

void Input::set_mouse_mode(i32 mode) {
	if(m_mouse_mode == mode) {
		return;
	}

	m_mouse_mode = mode;
	glfwSetInputMode(mp_window, GLFW_CURSOR, mode);
}

void Input::update_mouse_position(vec2 position) {
	m_mouse_position = position;
}

void Input::update_key(i32 key, bool is_pressed) {
	if(key < 0 || key > GLFW_KEY_LAST) [[unlikely]] {
		return;
	}

	m_keys[key] = is_pressed;
}

void Input::update_mouse_button(i32 button, bool is_pressed) {
	m_buttons[button] = is_pressed;
}
