#pragma once

class Camera {
public:
	Camera(vec3 position = vec3(0, 0, -1.0f), f32 fov = 80.0f);
	virtual ~Camera() {}

	void update(f32 dt);

	mat4 get_matrix(f32 aspect_ratio) const;

	vec3 get_forward_direction() const;
	vec3 get_right_direction() const;

	inline vec3 get_position() const { return m_position; }
	inline f32 get_pitch() const { return m_pitch; }
	inline f32 get_yaw() const { return m_yaw; }
	inline f32 get_fov() const { return m_fov; }

private:
	void handle_movement(f32 dt);
	void handle_mouse_movement();

private:
	vec3 m_position;
	f32 m_yaw = 0.0f;
	f32 m_pitch = 0.0f;
	f32 m_fov;
};
