#pragma once

struct InputState {
	bool move_left;
	bool move_right;
	bool move_forward;
	bool move_backward;

	bool jump;
	bool crouch;
};

class InputManager {
public:
	InputManager() = delete;

	static void handle_key_event(i32 key, bool is_pressed);
	static inline const InputState &get_state() { return s_state; }

private:
	static InputState s_state;
};
