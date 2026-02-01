#include "vox/client/player/player.hpp"
#include "vox/client/game.hpp"

#include "vox/common/system/tick_loop.hpp"
#include "vox/common/world/physics.hpp"
#include "vox/common/world/physics_constants.hpp"
#include "vox/client/core/input.hpp"

constexpr f32 FLY_CAMERA_FOV = 85.0f;
constexpr f32 DEFAULT_CAMERA_FOV = 75.0f;

constexpr f32 THIRD_PERSON_CAMERA_DISTANCE = 2.0f;

constexpr f32 TERMINAL_VELOCITY = 20.0f;

constexpr f32 PLAYER_HEAD_OFFSET = 0.1f;

constexpr f32 REACH_DISTANCE = 5.0f;

constexpr f32 JUMP_HEIGHT = 1.25f;
const f32 JUMP_FORCE = glm::sqrt(2.0f * JUMP_HEIGHT * GRAVITY);

constexpr f32 CAMERA_SENSITIVITY = 0.002f;

constexpr f32 GROUND_ACCELERATION = 80.0f;
constexpr f32 GROUND_FRICTION = 8.0f;

constexpr f32 MOVE_SPEED = 4.0f;

constexpr f32 AIR_ACCELERATION = 30.0f;
constexpr f32 AIR_FRICTION = 2.0f;

constexpr f32 FLY_ACCELERATION = 100.0f;
constexpr f32 FLY_MAX_SPEED = 8.0f;
constexpr f32 FLY_FRICTION = 5.0f;

constexpr vec3 UP = vec3(0.0f, 1.0f, 0.0f);

Player::Player(Camera &cam) : m_camera(cam), m_position(cam.m_position), m_height(PLAYER_HEIGHT) {
}

Player::~Player() {
}

void Player::tick(IWorld &world) {
	PROFILE_FUNC();

	m_prev_position = m_position;

	handle_crouch(world);
	m_target_height = m_is_crouching ? PLAYER_CROUCH_HEIGHT : PLAYER_HEIGHT;

	handle_movement(world, TickLoop::TICK_DURATION_SECONDS);

	if(Input::get_instance().get_mouse_mode() == GLFW_CURSOR_DISABLED) {
		handle_block_interaction(world);
	}

	// reset input, its handled for this tick
	m_input_state = PlayerInputState();
}

void Player::update(const IWorld &world, f64 alpha) {
	PROFILE_FUNC();

	const bool is_cursor_locked = Input::get_instance().get_mouse_mode() == GLFW_CURSOR_DISABLED;
	if(is_cursor_locked) {
		handle_input();
		handle_mouse_movement();
	}

	m_visual_position = glm::mix(m_prev_position, m_position, alpha);

	DebugRenderer &debug_renderer = Game::get_instance()->get_debug_renderer();
	debug_renderer.draw_aabb(calculate_aabb(), m_is_grounded ? vec3(1.0f, 0.0f, 0.0f) : vec3(1.0f, 0.0f, 1.0f));
	debug_renderer.draw_aabb(calculate_visual_aabb(), vec3(1.0f, 1.0f, 1.0f));

	m_height = glm::lerp(m_height, m_target_height, (f32)alpha);

	handle_camera_position(world, alpha);
}

AABB Player::calculate_aabb() const {
	return AABB {
        .min = m_position - vec3(PLAYER_HALF_WIDTH, 0, PLAYER_HALF_WIDTH),
        .max = m_position + vec3(PLAYER_HALF_WIDTH, m_height, PLAYER_HALF_WIDTH)
    };
}

AABB Player::calculate_visual_aabb() const {
	return AABB {
        .min = m_visual_position - vec3(PLAYER_HALF_WIDTH, 0, PLAYER_HALF_WIDTH),
        .max = m_visual_position + vec3(PLAYER_HALF_WIDTH, m_height, PLAYER_HALF_WIDTH)
	};
}

void Player::handle_input() {
	PROFILE_FUNC();

	Input &input = Input::get_instance();

	m_input_state.input_x = static_cast<f32>(input.is_key_pressed(GLFW_KEY_D)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_A));
	m_input_state.input_z = static_cast<f32>(input.is_key_pressed(GLFW_KEY_W)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_S));
	m_input_state.wish_to_crouch = input.is_key_pressed(GLFW_KEY_LEFT_CONTROL);
	m_input_state.wish_to_jump |= input.is_key_pressed(GLFW_KEY_SPACE);
	m_input_state.wish_to_sprint |= input.is_key_pressed(GLFW_KEY_LEFT_SHIFT);

	m_input_state.wish_to_place_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_RIGHT);
	m_input_state.wish_to_break_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_LEFT);
	m_input_state.wish_to_copy_block |= input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_MIDDLE);

	if(f32 scroll = input.get_scroll().y; scroll != 0.0f) {
		m_block_in_hand = static_cast<BlockID>(glm::mod(static_cast<i32>(m_block_in_hand) + static_cast<i32>(scroll), static_cast<i32>(BlockID::SIZE)));
	}

	if(input.is_key_just_pressed(GLFW_KEY_C)) {
		toggle_camera_mode();
	}
}

void Player::handle_camera_position(const IWorld &world, f32 alpha) {
	const f32 eye_offset = m_height - PLAYER_HEAD_OFFSET;
	const vec3 interpolated_position = m_visual_position + vec3(0.0f, eye_offset, 0.0f);

	switch(m_camera_mode) {
		case CameraMode::FirstPerson:
			m_camera.m_position = interpolated_position;
			break;

		case CameraMode::ThirdPerson:
			const vec3 direction = -m_camera.get_forward_direction();

			f32 distance = THIRD_PERSON_CAMERA_DISTANCE;
			if(auto result_opt = Physics::raycast(world, interpolated_position, direction, THIRD_PERSON_CAMERA_DISTANCE)) {
				distance = result_opt->distance;
			}

			m_camera.m_position = interpolated_position + direction * distance;

			break;
	}

	const f32 target_fov = m_fly_enabled ? FLY_CAMERA_FOV : DEFAULT_CAMERA_FOV;
	m_camera.m_fov = glm::mix(m_camera.m_fov, target_fov, alpha);
}

void Player::handle_movement(const IWorld &world, f32 dt) {
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

	// const auto move_axis = [&](u8 axis, f32 &velocity, f32 extent) -> bool {
	// 	if(glm::abs(velocity) < 0.0001f) {
	// 		return false;
	// 	}
	//
	// 	m_position[axis] += velocity * dt;
	//
	// 	constexpr f32 COLLISION_EPSILON = 0.001f;
	//
	// 	if(auto collision = Physics::check_collision(world, calculate_aabb())) {
	// 		m_position[axis] = collision->block_center[axis] - glm::sign(velocity) * (0.5f + extent + COLLISION_EPSILON);
	//
	// 		velocity = 0.0f;
	// 		return true;
	// 	}
	//
	// 	return false;
	// };
	//
	// move_axis(0, m_horizontal_velocity.x, PLAYER_HALF_WIDTH);
	// move_axis(2, m_horizontal_velocity.z, PLAYER_HALF_WIDTH);
	//
	// const bool is_falling = m_vertical_velocity < 0.0f;
	// if(!m_fly_enabled) {
	// 	if(move_axis(1, m_vertical_velocity, is_falling ? 0.0f : m_height)) {
	// 		m_is_grounded = is_falling;
	// 	} else {
	// 		m_is_grounded = false;
	// 	}
	// } else {
	// 	move_axis(1, m_horizontal_velocity.y, is_falling ? 0.0f : m_height);
	// }
	//
	AABB current_aabb = calculate_aabb();

	Physics::move_axis(world, X_AXIS, m_position.x, m_horizontal_velocity.x, current_aabb, PLAYER_HALF_WIDTH, dt);
	Physics::move_axis(world, Z_AXIS, m_position.z, m_horizontal_velocity.z, current_aabb, PLAYER_HALF_WIDTH, dt);

	const bool is_falling = m_vertical_velocity < 0.0f;
	const f32 y_extent = is_falling ? 0.0f : m_height;
	if(!m_fly_enabled) {
		if(Physics::move_axis(world, Y_AXIS, m_position.y, m_vertical_velocity, current_aabb, y_extent, dt)) {
			m_is_grounded = is_falling;
		} else {
			m_is_grounded = false;
		}
	} else {
		Physics::move_axis(world, Y_AXIS, m_position.y, m_horizontal_velocity.y, current_aabb, y_extent, dt);
	}
}

void Player::handle_crouch(const IWorld &world) {
	if(m_is_crouching == m_input_state.wish_to_crouch) {
		return;
	}
	
	// we can start crouching at any time
	if(!m_is_crouching) {
		m_is_crouching = true;
		return;
	}

	DebugRenderer &debug_renderer = Game::get_instance()->get_debug_renderer();
	debug_renderer.draw_line(m_position, m_position + vec3(0.0f, PLAYER_HEIGHT, 0.0f), vec3(0.0f, 0.0f, 1.0f));

	const AABB standing_aabb {
        .min = m_position - vec3(PLAYER_HALF_WIDTH, 0, PLAYER_HALF_WIDTH),
        .max = m_position + vec3(PLAYER_HALF_WIDTH, PLAYER_HEIGHT, PLAYER_HALF_WIDTH)
    };

	// player would clip his head
	if(Physics::check_collision(world, standing_aabb)) {
		return;
	}

	m_is_crouching = false;
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

void Player::toggle_camera_mode() {
	switch(m_camera_mode) {
		case CameraMode::FirstPerson:
			m_camera_mode = CameraMode::ThirdPerson;
			break;

		case CameraMode::ThirdPerson:
			m_camera_mode = CameraMode::FirstPerson;
			break;
	}
}
