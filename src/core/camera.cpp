#include "camera.hpp"
#include "glm/gtc/constants.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

constexpr f32 SENSITIVITY = 0.002f;
constexpr f32 MOVE_SPEED = 5.0f;
constexpr f32 NEAR_PLANE = 0.1f;
constexpr f32 FAR_PLANE = 100.0f;

Camera::Camera(vec3 position, f32 fov) 
: m_position(position), m_fov(fov) {
}

void Camera::update(f32 dt) {
	handle_mouse_movement();
	handle_movement(dt);
}

mat4 Camera::get_matrix(f32 aspect_ratio) const {
	constexpr vec3 UP = vec3(0.0f, 1.0f, 0.0f);
	const vec3 forward = get_forward_direction();

	const mat4 projection_matrix = glm::perspective(glm::radians(m_fov), aspect_ratio, NEAR_PLANE, FAR_PLANE);
	const mat4 view_matrix = glm::lookAt(m_position, m_position + forward, UP);

	return projection_matrix * view_matrix;
}

vec3 Camera::get_forward_direction() const {
	const f32 yaw = m_yaw + glm::half_pi<f32>();
	const f32 pitch_cos = cos(m_pitch);

	return vec3(
		pitch_cos * cos(yaw), 
		sin(m_pitch), 
		pitch_cos * sin(yaw)
	);
}

vec3 Camera::get_right_direction() const {
	return glm::normalize(glm::cross(get_forward_direction(), vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::handle_movement(f32 dt) {
	const InputState &input = InputManager::get_instance().get_state();

	vec3 movement = vec3(0.0f);

	vec3 forward = get_forward_direction();
	forward.y = 0.0f;
	forward = glm::normalize(forward);

	const vec3 right = get_right_direction();

	if(input.move_left)		movement -= right;
	if(input.move_right)	movement += right;
	if(input.move_forward)	movement += forward;
	if(input.move_backward) movement -= forward;
	if(input.jump)			movement += vec3(0.0f, 1.0f, 0.0f);
	if(input.crouch)		movement -= vec3(0.0f, 1.0f, 0.0f);

	if(movement.x == 0.0f && movement.y == 0.0f && movement.z == 0.0f) {
		return;
	}

	movement = glm::normalize(movement);
	m_position += movement * MOVE_SPEED * dt;
}

void Camera::handle_mouse_movement() {
	InputManager &input_manager = InputManager::get_instance();

	if(input_manager.get_mouse_mode() != GLFW_CURSOR_DISABLED) {
		return;
	}

	const InputState &input = input_manager.get_state();

	m_yaw += input.mouse_delta.x * SENSITIVITY;
	m_pitch -= input.mouse_delta.y * SENSITIVITY;

	m_pitch = glm::clamp(m_pitch, -glm::half_pi<f32>() + glm::radians(1.0f), glm::half_pi<f32>() - glm::radians(1.0f));
	m_yaw = glm::mod(m_yaw, glm::two_pi<f32>());
}
