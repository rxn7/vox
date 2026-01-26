#pragma once

class Camera {
public:
	Camera(vec3 position = vec3(0, 0, -1.0f), f32 fov = 80.0f);
	virtual ~Camera() {}

	mat4 get_projection_matrix(f32 aspect_ratio) const;
	mat4 get_view_matrix() const;

	vec3 get_forward_direction() const;
	vec3 get_right_direction() const;

public:
	vec3 m_position;
	f32 m_yaw = 0.0f;
	f32 m_pitch = 0.0f;
	f32 m_fov;
};
