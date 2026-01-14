#include "input.hpp"

InputState InputManager::s_state = {};

void InputManager::handle_key_event(i32 key, bool is_pressed) {
	switch(key) {
		case GLFW_KEY_A: s_state.move_left = is_pressed; break;
		case GLFW_KEY_D: s_state.move_right = is_pressed; break;
		case GLFW_KEY_W: s_state.move_forward = is_pressed; break;
		case GLFW_KEY_S: s_state.move_backward = is_pressed; break;
		case GLFW_KEY_SPACE: s_state.jump = is_pressed; break;
		case GLFW_KEY_LEFT_SHIFT: s_state.crouch = is_pressed; break;
	}
}
