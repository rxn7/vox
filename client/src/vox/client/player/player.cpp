#include "vox/client/player/player.hpp"

#include "vox/common/system/tick_loop.hpp"
#include "vox/client/core/input.hpp"
#include "vox/common/world/block_registry.hpp"
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

	const vec3 visual_position = glm::mix(m_prev_position, m_position, alpha);

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
		.min = vec3(m_position.x - HALF_PLAYER_WIDTH, m_position.y - PLAYER_HEIGHT, m_position.z - HALF_PLAYER_WIDTH),
																/* + 0.1f to prevent camera clipping through the ceiling */
		.max = vec3(m_position.x + HALF_PLAYER_WIDTH, m_position.y + 0.1f, m_position.z + HALF_PLAYER_WIDTH)
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

	const auto horizontal_move = [&](u8 axis) {
		m_position[axis] += m_horizontal_velocity[axis] * dt;
		if(check_collision(world)) {
			m_position[axis] -= m_horizontal_velocity[axis] * dt;
			m_horizontal_velocity[axis] = 0.0f;
		}
	};

	horizontal_move(0);
	horizontal_move(2);

	if(!m_fly_enabled) {
		const f32 dy = m_vertical_velocity * dt;
		m_position.y += dy;
		if(check_collision(world)) {
			m_position.y -= dy;

			if(m_vertical_velocity < 0.0f) {
				const f32 current_feet_y = m_position.y - PLAYER_HEIGHT;
				const f32 target_feet_y = glm::floor(current_feet_y);

				const f32 snap_y = target_feet_y + PLAYER_HEIGHT + 0.0001f;
				m_position.y = snap_y;

				m_is_grounded = true;
				m_vertical_velocity = 0.0f;
			} else if(m_vertical_velocity > 0.0f) {
				m_vertical_velocity = 0.0f;
			}
		} else {
			m_is_grounded = false;
		}
	} else {
		m_position.y += m_horizontal_velocity.y * dt;
		if(check_collision(world)) {
			m_position.y -= m_horizontal_velocity.y * dt;
			m_horizontal_velocity.y = 0.0f;
		}
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

bool Player::check_collision(IWorld &world) {
	PROFILE_FUNC();

	const AABB aabb = calculate_aabb();

	const i32 min_x = static_cast<i32>(glm::floor(aabb.min.x));
	const i32 max_x = static_cast<i32>(glm::floor(aabb.max.x));

	const i32 min_y = static_cast<i32>(glm::floor(aabb.min.y));
	const i32 max_y = static_cast<i32>(glm::floor(aabb.max.y));

	const i32 min_z = static_cast<i32>(glm::floor(aabb.min.z));
	const i32 max_z = static_cast<i32>(glm::floor(aabb.max.z));

	for(i32 x = min_x; x <= max_x; ++x) {
		for(i32 y = min_y; y <= max_y; ++y) {
			for(i32 z = min_z; z <= max_z; ++z) {
				const BlockPosition check_position = BlockPosition(vec3(x, y, z) + 0.5f);
				const BlockID block_id = world.get_block(check_position);
				const BlockType &block_type = BlockRegistry::get(block_id);

				if(block_type.is_solid()) {
					return true;
				}
			}
		}
	}

	return false;
}

void Player::handle_block_interaction(IWorld &world) {
	PROFILE_FUNC();

	const vec3 ray_start = m_position;
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
