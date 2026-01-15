#pragma once

class Camera {
public:
	Camera(vec3 position = vec3(0, 0, -1.0f), f32 fov = 80.0f);
	virtual ~Camera() {}

	void update(f32 dt);

	[[nodiscard]] mat4 get_matrix(f32 aspect_ratio) const;

	[[nodiscard]] vec3 get_forward_direction() const;
	[[nodiscard]] vec3 get_right_direction() const;

	[[nodiscard]] inline const vec3 &get_position() const { return m_position; }
	[[nodiscard]] inline f32 get_pitch() const { return m_pitch; }
	[[nodiscard]] inline f32 get_yaw() const { return m_yaw; }
	[[nodiscard]] inline f32 get_fov() const { return m_fov; }

private:
	void handle_movement(f32 dt);
	void handle_mouse_movement();

private:
	vec3 m_position;
	f32 m_yaw = 0.0f;
	f32 m_pitch = 0.0f;
	f32 m_fov;
};
