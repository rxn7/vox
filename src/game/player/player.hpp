#pragma once

#include "world/world.hpp"
#include "world/aabb.hpp"
#include "graphics/backend/camera.hpp"

constexpr f32 GRAVITY = 9.81f;
constexpr f32 TERMINAL_VELOCITY = 20.0f;
constexpr f32 PLAYER_HEIGHT = 1.6f;
constexpr f32 PLAYER_WIDTH = 0.3f;

constexpr f32 REACH_DISTANCE = 5.0f;

constexpr f32 JUMP_HEIGHT = 1.25f;
const f32 JUMP_FORCE = std::sqrt(2.0f * JUMP_HEIGHT * GRAVITY);

constexpr f32 SENSITIVITY = 0.002f;
constexpr f32 MOVE_SPEED = 4.0f;

class Player {
public:
	Player(Camera &cam);
	~Player();

	void update(World &world, f32 dt);
	AABB calculate_aabb() const;

private:
	void handle_movement(World &world, f32 dt);
	void handle_mouse_movement();
	bool check_collision(World &world);
	void handle_block_interaction(World &world); 

public:
	bool m_fly_enabled = false;

private:
	Camera &m_camera;
	vec3 &m_position;

	f32 m_vertical_velocity = 0.0f;
	vec3 m_horizontal_velocity;

	bool m_is_grounded = false;
};
