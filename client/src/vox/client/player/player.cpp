#include "vox/client/player/player.hpp"

#include "vox/common/system/tick_loop.hpp"
#include "vox/client/core/input.hpp"
#include "vox/common/world/physics.hpp"

constexpr f32 HALF_PLAYER_WIDTH = PLAYER_WIDTH / 2.0f;
constexpr vec3 UP = vec3(0.0f, 1.0f, 0.0f);

Player::Player(Camera &cam) : m_camera(cam), m_position(cam.m_position) {
}

Player::~Player() {
}

void Player::tick(IWorld &world) {
	PROFILE_FUNC();

	m_prev_position = m_position;

	handle_movement(world, TickLoop::TICK_DURATION_SECONDS);

	if(Input::get_instance().get_mouse_mode() == GLFW_CURSOR_DISABLED) {
		handle_block_interaction(world);
	}

	// reset input, its handled for this tick
	m_input_state = PlayerInputState();
}

void Player::update(f64 alpha) {
	PROFILE_FUNC();

	const vec3 visual_position = glm::mix(m_prev_position, m_position, alpha) + vec3(0.0f, PLAYER_EYE_OFFSET, 0.0f);
	m_camera.m_position = visual_position;

	const f32 target_fov = m_fly_enabled ? FLY_CAMERA_FOV : DEFAULT_CAMERA_FOV;
	m_camera.m_fov = glm::mix(m_camera.m_fov, target_fov, static_cast<f32>(alpha));

	if(Input::get_instance().get_mouse_mode() != GLFW_CURSOR_DISABLED) {
		return;
	}

	handle_input();
	handle_mouse_movement();
}

AABB Player::calculate_aabb() const {
	return AABB {
        .min = m_position - vec3(PLAYER_HALF_WIDTH, PLAYER_HALF_HEIGHT, PLAYER_HALF_WIDTH),
        .max = m_position + vec3(PLAYER_HALF_WIDTH, PLAYER_HALF_HEIGHT, PLAYER_HALF_WIDTH)
    };
}

void Player::handle_input() {
	PROFILE_FUNC();

	Input &input = Input::get_instance();

	m_input_state.input_x = static_cast<f32>(input.is_key_pressed(GLFW_KEY_D)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_A));
	m_input_state.input_z = static_cast<f32>(input.is_key_pressed(GLFW_KEY_W)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_S));
	m_input_state.wish_to_jump |= input.is_key_pressed(GLFW_KEY_SPACE);
	m_input_state.wish_to_sprint |= input.is_key_pressed(GLFW_KEY_LEFT_SHIFT);

	m_input_state.wish_to_place_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_RIGHT);
	m_input_state.wish_to_break_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_LEFT);
	m_input_state.wish_to_copy_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_MIDDLE);

	if(f32 scroll = input.get_scroll().y; scroll != 0.0f) {
		m_block_in_hand = static_cast<BlockID>(glm::mod(static_cast<i32>(m_block_in_hand) + static_cast<i32>(scroll), static_cast<i32>(BlockID::SIZE)));
	}
}

void Player::handle_movement(IWorld &world, f32 dt) {
	PROFILE_FUNC();

	vec3 forward = m_camera.get_forward_direction();
	forward.y = 0.0f;
	forward = glm::normalize(forward);

	const vec3 right = m_camera.get_right_direction();

	const Input &input = Input::get_instance();

	vec3 wish_dir = forward * m_input_state.input_z + right * m_input_state.input_x;
	if(glm::length2(wish_dir) > 0.0001f) {
		wish_dir = glm::normalize(wish_dir);
	}
	
	if(m_fly_enabled) {
		m_vertical_velocity = 0.0f;

		wish_dir.y = static_cast<f32>(input.is_key_pressed(GLFW_KEY_SPACE)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_LEFT_SHIFT));
		if(glm::length2(wish_dir) > 0.0001f) {
			wish_dir = glm::normalize(wish_dir);
		}
		
		accelerate(wish_dir, FLY_ACCELERATION, FLY_MAX_SPEED, dt);
		apply_friction(FLY_FRICTION, dt);
	} else {
		if(m_is_grounded) {
			accelerate(wish_dir, GROUND_ACCELERATION, MOVE_SPEED, dt);
			apply_friction(GROUND_FRICTION, dt);
		} else {
			accelerate(wish_dir, AIR_ACCELERATION, MOVE_SPEED, dt);
			apply_friction(AIR_FRICTION, dt);
		}
		
		m_vertical_velocity -= GRAVITY * dt;
		m_vertical_velocity = std::max(m_vertical_velocity, -TERMINAL_VELOCITY);
		
		if(m_is_grounded && input.is_key_pressed(GLFW_KEY_SPACE)) {
			m_vertical_velocity = JUMP_FORCE;
			m_is_grounded = false;
		}
	}

	const auto move_axis = [&](u8 axis, f32 &velocity, f32 half_size) -> bool {
		if(glm::abs(velocity) < 0.0001f) {
			return false;
		}

		m_position[axis] += velocity * dt;

		constexpr f32 COLLISION_EPSILON = 0.001f;

		if(auto collision = Physics::check_collision(world, calculate_aabb())) {
			m_position[axis] = collision->block_center[axis] - glm::sign(velocity) * (0.5f + half_size + COLLISION_EPSILON);
			velocity = 0.0f;
			return true;
		}

		return false;
	};

	move_axis(0, m_horizontal_velocity.x, PLAYER_HALF_WIDTH);
	move_axis(2, m_horizontal_velocity.z, PLAYER_HALF_WIDTH);

	if(!m_fly_enabled) {
		const bool is_falling = m_vertical_velocity < 0.0f;

		if(move_axis(1, m_vertical_velocity, PLAYER_HALF_HEIGHT)) {
			m_is_grounded = is_falling;
		} else {
			m_is_grounded = false;
		}
	} else {
		move_axis(1, m_horizontal_velocity.y, PLAYER_HALF_HEIGHT);
	}
}

void Player::handle_mouse_movement() {
	PROFILE_FUNC();

	Input &input = Input::get_instance();

	if(input.get_mouse_mode() != GLFW_CURSOR_DISABLED) {
		return;
	}

	const vec2 mouse_delta = input.get_mouse_delta();
	m_camera.m_yaw += mouse_delta.x * CAMERA_SENSITIVITY;
	m_camera.m_pitch -= mouse_delta.y * CAMERA_SENSITIVITY;

	m_camera.m_pitch = glm::clamp(m_camera.m_pitch, -glm::half_pi<f32>() + glm::radians(1.0f), glm::half_pi<f32>() - glm::radians(1.0f));
	m_camera.m_yaw = glm::mod(m_camera.m_yaw, glm::two_pi<f32>());
}

void Player::accelerate(vec3 wish_dir, f32 acceleration, f32 max_speed, f32 dt) {
	const f32 current_speed = glm::dot(m_horizontal_velocity, wish_dir);
	const f32 add_speed = max_speed - current_speed;
	
	if(add_speed <= 0.0f) {
		return;
	}
	
	const f32 accel_speed = glm::min(acceleration * dt, add_speed);

	m_horizontal_velocity += wish_dir * accel_speed;
}

void Player::apply_friction(f32 friction, f32 dt) {
	const f32 speed = m_horizontal_velocity.length();
	if(speed < 0.0001f) {
		m_horizontal_velocity = vec3(0.0f);
		return;
	}
	
	const f32 drop = speed * friction * dt;
	const f32 new_speed = glm::max(0.0f, speed - drop);
	
	m_horizontal_velocity *= (new_speed / speed);
}

void Player::handle_block_interaction(IWorld &world) {
	PROFILE_FUNC();

	const vec3 ray_start = m_camera.m_position;
	const vec3 ray_dir = m_camera.get_forward_direction();

	const std::optional<Physics::RaycastResult> raycast_result = Physics::raycast(world, ray_start, ray_dir, REACH_DISTANCE);
		
	if(!raycast_result.has_value()) {
		m_last_highlighted_block_position = std::nullopt;
		return;
	}

	if(m_input_state.wish_to_copy_block) {
		m_block_in_hand = world.get_block(raycast_result->hit_block_position);
		if(m_block_in_hand == BlockID::Air) {
			m_block_in_hand = BlockID::Stone;
		}
	}

	m_last_highlighted_block_position = raycast_result->hit_block_position;

	if(m_input_state.wish_to_place_block) {
		const AABB player_aabb = calculate_aabb();
		const AABB block_aabb = {
			.min = vec3(raycast_result->previous_grid_position),
			.max = vec3(raycast_result->previous_grid_position) + 1.0f,
		};

		if(!player_aabb.overlap(block_aabb)) {
			const BlockPosition place_position(vec3(raycast_result->previous_grid_position) + 0.5f);
			
			// TODO: Send place packet to server
			world.set_block(place_position, m_block_in_hand);
		}
	} else if(m_input_state.wish_to_break_block) {
		// TODO: Send break packet to server
		world.set_block(raycast_result->hit_block_position, BlockID::Air);
	}
}
