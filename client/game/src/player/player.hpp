#pragma once

#include "vox/common/world/world.hpp"
#include "vox/common/world/aabb.hpp"
#include "vox/common/world/physics_constants.hpp"
#include "vox/engine/graphics/backend/camera.hpp"

constexpr f32 TERMINAL_VELOCITY = 20.0f;
constexpr f32 PLAYER_HEIGHT = 1.6f;
constexpr f32 PLAYER_WIDTH = 0.3f;
constexpr f32 REACH_DISTANCE = 5.0f;

constexpr f32 JUMP_HEIGHT = 1.25f;
const f32 JUMP_FORCE = std::sqrt(2.0f * JUMP_HEIGHT * GRAVITY);

constexpr f32 CAMERA_SENSITIVITY = 0.002f;

constexpr f32 GROUND_ACCELERATION = 80.0f;
constexpr f32 GROUND_FRICTION = 10.0f;

constexpr f32 MOVE_SPEED = 3.0f;

constexpr f32 AIR_ACCELERATION = 30.0f;
constexpr f32 AIR_FRICTION = 5.0f;

constexpr f32 FLY_ACCELERATION = 100.0f;
constexpr f32 FLY_MAX_SPEED = 8.0f;
constexpr f32 FLY_FRICTION = 5.0f;

class Player {
public:
	Player(Camera &cam);
	~Player();

	void update(World &world, f32 dt);
	AABB calculate_aabb() const;
	const std::optional<BlockPosition> &get_last_highlighted_block_position() const { return m_last_highlighted_block_position; }

private:
	void handle_movement(World &world, f32 dt);
	void handle_mouse_movement();
    void accelerate(vec3 wish_dir, f32 acceleration, f32 max_speed, f32 dt);
    void apply_friction(f32 friction, f32 dt);
	bool check_collision(World &world);
	void handle_block_interaction(World &world); 

public:
	bool m_fly_enabled = false;

private:
	Camera &m_camera;
	vec3 &m_position;

	f32 m_vertical_velocity = 0.0f;
	vec3 m_horizontal_velocity;

	std::optional<BlockPosition> m_last_highlighted_block_position;

	bool m_is_grounded = false;
};
