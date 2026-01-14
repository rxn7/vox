#pragma once

#include "core/input.hpp"

class Camera {
public:
	Camera(vec3 position = vec3(0, 0, -1.0f), vec3 direction = vec3(0.0f, 0.0f, -1.0f), f32 fov = 80.0f);
	virtual ~Camera() {}

	void update(f32 dt, const InputState &input);
	mat4 get_matrix(f32 aspect_ratio) const;

private:
	void handle_movement(f32 dt, const InputState &input);

public:
	vec3 m_position;
	vec3 m_direction;
	f32 m_fov;
};
