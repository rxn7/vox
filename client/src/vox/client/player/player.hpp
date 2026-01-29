#pragma once

#include "vox/common/world/i_world.hpp"
#include "vox/common/world/aabb.hpp"
#include "vox/common/world/physics_constants.hpp"
#include "vox/client/graphics/backend/camera.hpp"

constexpr f32 FLY_CAMERA_FOV = 85.0f;
constexpr f32 DEFAULT_CAMERA_FOV = 75.0f;

constexpr f32 TERMINAL_VELOCITY = 20.0f;
constexpr f32 PLAYER_HEIGHT = 1.6f;
constexpr f32 PLAYER_WIDTH = 0.3f;
constexpr f32 REACH_DISTANCE = 5.0f;

constexpr f32 JUMP_HEIGHT = 1.25f;
const f32 JUMP_FORCE = std::sqrt(2.0f * JUMP_HEIGHT * GRAVITY);

constexpr f32 CAMERA_SENSITIVITY = 0.002f;

constexpr f32 GROUND_ACCELERATION = 80.0f;
constexpr f32 GROUND_FRICTION = 8.0f;

constexpr f32 MOVE_SPEED = 4.0f;

constexpr f32 AIR_ACCELERATION = 30.0f;
constexpr f32 AIR_FRICTION = 2.0f;

constexpr f32 FLY_ACCELERATION = 100.0f;
constexpr f32 FLY_MAX_SPEED = 8.0f;
constexpr f32 FLY_FRICTION = 5.0f;

struct PlayerInputState {
	f32 input_x = 0.0f;
	f32 input_z = 0.0f;
	bool wish_to_jump = false;

	bool wish_to_sprint = false;

	bool wish_to_place_block = false;
	bool wish_to_break_block = false;
	bool wish_to_copy_block = false;
};

class Player {
public:
	Player(Camera &cam);
	~Player();

	void tick(IWorld &world);
	void update(f64 alpha);
	AABB calculate_aabb() const;

	// physics position
	vec3 get_position() const { 
		return m_position; 
	}

	BlockID get_block_in_hand() const {
		return m_block_in_hand;
	}

	const std::optional<BlockPosition> &get_last_highlighted_block_position() const { 
		return m_last_highlighted_block_position; 
	}

private:
	void handle_input();
	void handle_movement(IWorld &world, f32 dt);
	void handle_mouse_movement();
	void accelerate(vec3 wish_dir, f32 acceleration, f32 max_speed, f32 dt);
	void apply_friction(f32 friction, f32 dt);
	bool check_collision(IWorld &world);
	void handle_block_interaction(IWorld &world); 

public:
	bool m_fly_enabled = false;

private:
	Camera &m_camera;
	PlayerInputState m_input_state;

	vec3 m_prev_position;
	vec3 m_position;

	f32 m_vertical_velocity = 0.0f;
	vec3 m_horizontal_velocity = vec3(0.0f);

	BlockID m_block_in_hand = BlockID::Stone;
	std::optional<BlockPosition> m_last_highlighted_block_position;

	bool m_is_grounded = false;
};
