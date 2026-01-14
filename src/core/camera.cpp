#include "camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

constexpr f32 MOVE_SPEED = 5.0f;
constexpr f32 NEAR_PLANE = 0.1f;
constexpr f32 FAR_PLANE = 100.0f;

Camera::Camera(vec3 position, vec3 direction, f32 fov) 
: m_position(position), m_direction(direction), m_fov(fov) {
}

void Camera::update(f32 dt, const InputState &input) {
	handle_movement(dt, input);
}

void Camera::handle_movement(f32 dt, const InputState &input) {
	vec3 movement = vec3(0.0f);

	if(input.move_left)		movement.x -= 1.0f;
	if(input.move_right)	movement.x += 1.0f;
	if(input.move_forward)	movement.z -= 1.0f;
	if(input.move_backward) movement.z += 1.0f;
	if(input.jump)			movement.y += 1.0f;
	if(input.crouch)		movement.y -= 1.0f;

	if(movement.x == 0.0f && movement.y == 0.0f && movement.z == 0.0f) {
		return;
	}

	movement = glm::normalize(movement);
	m_position += movement * MOVE_SPEED * dt;
}

mat4 Camera::get_matrix(f32 aspect_ratio) const {
	constexpr vec3 UP = vec3(0.0f, 1.0f, 0.0f);

	const mat4 projection_matrix = glm::perspective(glm::radians(m_fov), aspect_ratio, NEAR_PLANE, FAR_PLANE);
	const mat4 view_matrix = glm::lookAt(m_position, m_position + m_direction, UP);

	return projection_matrix * view_matrix;
}
