#pragma once

#include "core/input.hpp"

class Camera {
public:
	Camera(vec3 position = vec3(0, 0, -1.0f), f32 fov = 80.0f);
	virtual ~Camera() {}

	void update(f32 dt);

	mat4 get_matrix(f32 aspect_ratio) const;

	vec3 get_forward_direction() const;
	vec3 get_right_direction() const;

private:
	void handle_movement(f32 dt);
	void handle_mouse_movement();

public:
	vec3 m_position;
	f32 m_yaw;
	f32 m_pitch;
	f32 m_fov;
};
