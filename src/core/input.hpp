#pragma once

struct InputState {
	bool move_left = false;
	bool move_right = false;
	bool move_forward = false;
	bool move_backward = false;

	bool jump = false;
	bool crouch = false;

	vec2 mouse_delta = vec2(0);
};

class InputManager {
public:
	InputManager() {}

	inline static InputManager &get_instance() {
		if(!sp_instance) [[unlikely]] {
			sp_instance = std::make_unique<InputManager>();
		}

		return *sp_instance.get();
	}

	void set_mouse_mode(GLFWwindow *window, i32 mode);
	void update_mouse_position(vec2 position);
	void handle_key_event(i32 key, bool is_pressed);
	inline i32 get_mouse_mode() { return m_mouse_mode; }
	inline const InputState &get_state() { return m_state; }

private:
	static std::unique_ptr<InputManager> sp_instance;

	std::optional<vec2> m_last_mouse_position = std::nullopt;
	i32 m_mouse_mode = GLFW_CURSOR_NORMAL;
	InputState m_state = {0};
};
