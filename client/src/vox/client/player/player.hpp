#pragma once

#include "vox/common/world/i_world.hpp"
#include "vox/common/world/aabb.hpp"
#include "vox/client/graphics/backend/camera.hpp"
#include "vox/client/player/player_model.hpp"

constexpr f32 PLAYER_HEIGHT = 1.8f;
constexpr f32 PLAYER_CROUCH_HEIGHT = 1.3f;
constexpr f32 PLAYER_WALK_SPEED = 4.0f;

constexpr f32 PLAYER_WIDTH = 0.3f;
constexpr f32 PLAYER_HALF_WIDTH = PLAYER_WIDTH * 0.5f;

enum class CameraMode {
	FirstPerson,
	ThirdPerson
};

struct PlayerInputState {
	f32 input_x = 0.0f;
	f32 input_z = 0.0f;

	bool wish_to_crouch = false;

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
	void update(const IWorld &world, f64 alpha);
	void render();
	AABB calculate_aabb() const;
	AABB calculate_visual_aabb() const;
	
	// physics position
	inline vec3 get_position() const { 
		return m_position; 
	}

	inline BlockID get_block_in_hand() const {
		return m_block_in_hand;
	}

	inline bool is_grounded() const {
		return m_is_grounded;
	}

	inline const std::optional<BlockPosition> &get_last_highlighted_block_position() const { 
		return m_last_highlighted_block_position; 
	}

private:
	void handle_camera_position(const IWorld &world, f32 alpha);
	void handle_input();
	void handle_movement(const IWorld &world, f32 dt);
	void handle_crouch(const IWorld &world);
	void handle_mouse_movement();
	void accelerate(vec3 wish_dir, f32 acceleration, f32 max_speed, f32 dt);
	void apply_friction(f32 friction, f32 dt);
	void handle_block_interaction(IWorld &world); 
	void toggle_camera_mode();

public:
	bool m_fly_enabled = false;

private:
	Camera &m_camera;
	CameraMode m_camera_mode = CameraMode::FirstPerson;

	PlayerModel m_model;

	PlayerInputState m_input_state;

	vec3 m_prev_position;
	vec3 m_position;

	vec3 m_visual_position;

	f32 m_target_height;
	f32 m_height;

	f32 m_vertical_velocity = 0.0f;
	vec3 m_horizontal_velocity = vec3(0.0f);

	BlockID m_block_in_hand = BlockID::Stone;
	std::optional<BlockPosition> m_last_highlighted_block_position;

	bool m_is_grounded = false;
	bool m_is_crouching = false;
};
