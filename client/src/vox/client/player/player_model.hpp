#pragma once

#include "vox/client/graphics/model_part.hpp"

class PlayerModel {
public:
	PlayerModel();

	void update(vec3 position, vec3 velocity, f32 pitch, f32 yaw);
	void render(vec3 position);
	void build_geometry();

private:
	void handle_body_yaw(vec3 vel, f32 speed, f32 camera_yaw);
	void handle_head_rotation(f32 pitch, f32 yaw);
	void handle_walk_animation(f32 speed);

private:
	f32 m_walk_time = 0.0f;
	f32 m_swing = 0.0f;
	f32 m_body_yaw = 0.0f;

	ModelPart m_torso;
	ModelPart m_head;
	ModelPart m_left_arm;
	ModelPart m_right_arm;
	ModelPart m_left_leg;
	ModelPart m_right_leg;
};
