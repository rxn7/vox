#include "player/player.hpp"

#include "vox/engine/core/input.hpp"
#include "vox/common/world/block_registry.hpp"
#include "vox/common/world/physics_constants.hpp"
#include "vox/common/world/world.hpp"

constexpr f32 HALF_PLAYER_WIDTH = PLAYER_WIDTH / 2.0f;
constexpr vec3 UP = vec3(0.0f, 1.0f, 0.0f);

Player::Player(Camera &cam) : m_camera(cam), m_position(cam.m_position) {
}

Player::~Player() {
}

void Player::update(World &world, f32 dt) {
	PROFILE_FUNC();

	handle_movement(world, dt);
	handle_mouse_movement();
	handle_block_interaction(world);
}

AABB Player::calculate_aabb() const {
	return AABB {
		.min = vec3(m_position.x - HALF_PLAYER_WIDTH, m_position.y - PLAYER_HEIGHT, m_position.z - HALF_PLAYER_WIDTH),
																/* + 0.2f to prevent camera clipping through the ceiling */
		.max = vec3(m_position.x + HALF_PLAYER_WIDTH, m_position.y + 0.2f, m_position.z + HALF_PLAYER_WIDTH)
	};
}

void Player::handle_movement(World &world, f32 dt) {
	PROFILE_FUNC();

	vec3 forward = m_camera.get_forward_direction();
	forward.y = 0.0f;
	forward = glm::normalize(forward);

	const vec3 right = m_camera.get_right_direction();

	const Input &input = Input::get_instance();

	const f32 input_x = static_cast<f32>(input.is_key_pressed(GLFW_KEY_D)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_A));
	const f32 input_z = static_cast<f32>(input.is_key_pressed(GLFW_KEY_W)) - static_cast<f32>(input.is_key_pressed(GLFW_KEY_S));

	vec3 wish_dir = forward * input_z + right * input_x;
	if(glm::length2(wish_dir) > 0.0001f) {
		wish_dir = glm::normalize(wish_dir);
	}
    
    if(m_fly_enabled) {
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
        m_position.y += m_vertical_velocity * dt;
        if(check_collision(world)) {
            m_position.y -= m_vertical_velocity * dt;

            if(m_vertical_velocity < 0.0f) {
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

bool Player::check_collision(World &world) {
	PROFILE_FUNC();

	const AABB aabb = calculate_aabb();

	const i32 min_x = static_cast<i32>(std::floor(aabb.min.x));
	const i32 max_x = static_cast<i32>(std::floor(aabb.max.x));

	const i32 min_y = static_cast<i32>(std::floor(aabb.min.y));
	const i32 max_y = static_cast<i32>(std::floor(aabb.max.y));

	const i32 min_z = static_cast<i32>(std::floor(aabb.min.z));
	const i32 max_z = static_cast<i32>(std::floor(aabb.max.z));

	for(i32 x = min_x; x <= max_x; ++x) {
		for(i32 y = min_y; y <= max_y; ++y) {
			for(i32 z = min_z; z <= max_z; ++z) {
				const BlockPosition check_position(vec3(x, y, z) + 0.5f);
				const BlockID block_id = world.get_block(check_position);
				const BlockType &block_type = BlockRegistry::get(block_id);

				if(block_type.is_solid) {
					return true;
				}
			}
		}
	}

	return false;
}

void Player::handle_block_interaction(World &world) {
	PROFILE_FUNC();

	Input &input = Input::get_instance();
    
    const bool mouse_locked = input.get_mouse_mode() == GLFW_CURSOR_DISABLED;
	const bool wish_to_break = mouse_locked && input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_LEFT);
	const bool wish_to_place = mouse_locked && input.is_mouse_button_just_pressed(GLFW_MOUSE_BUTTON_RIGHT);

	const vec3 ray_start = m_position;
	const vec3 ray_dir = m_camera.get_forward_direction();

	const RaycastResult raycast_result = world.raycast(ray_start, ray_dir, REACH_DISTANCE);
		
	if(!raycast_result.hit) {
		m_last_highlighted_block_position = std::nullopt;
		return;
	}

	m_last_highlighted_block_position = raycast_result.hit_block_position;

	if(wish_to_place) {
		const AABB player_aabb = calculate_aabb();
		const AABB block_aabb = {
			.min = vec3(raycast_result.previous_grid_position) - 0.5f,
			.max = vec3(raycast_result.previous_grid_position) + 1.0f,
		};

		if(!player_aabb.overlap(block_aabb)) {
			const BlockPosition place_position(vec3(raycast_result.previous_grid_position) + 0.5f);
            
            // TODO: Send place packet to server
			world.set_block(place_position, BlockID::Stone);
		}
	} else if(wish_to_break) {
        // TODO: Send break packet to server
		world.set_block(raycast_result.hit_block_position, BlockID::Air);
	}
}
