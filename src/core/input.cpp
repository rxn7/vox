#include "input.hpp"

SINGLETON_IMPL(InputManager);

void InputManager::set_mouse_mode(GLFWwindow *window, i32 mode) {
	if(m_mouse_mode == mode) {
		return;
	}

	m_mouse_mode = mode;
	glfwSetInputMode(window, GLFW_CURSOR, mode);
}

void InputManager::update_mouse_position(vec2 position) {
	if(m_last_mouse_position) [[likely]] {
		m_state.mouse_delta = position - *m_last_mouse_position;
	}

	m_last_mouse_position = position;
}

void InputManager::handle_key_event(i32 key, bool is_pressed) {
	switch(key) {
		case GLFW_KEY_A: m_state.move_left = is_pressed; break;
		case GLFW_KEY_D: m_state.move_right = is_pressed; break;
		case GLFW_KEY_W: m_state.move_forward = is_pressed; break;
		case GLFW_KEY_S: m_state.move_backward = is_pressed; break;
		case GLFW_KEY_SPACE: m_state.jump = is_pressed; break;
		case GLFW_KEY_LEFT_SHIFT: m_state.crouch = is_pressed; break;
	}
}
