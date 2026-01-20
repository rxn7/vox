#include "core/camera.hpp"
#include "core/input.hpp"
#include "tools/profiler/scope_timer.hpp"

constexpr f32 SENSITIVITY = 0.002f;
constexpr f32 MOVE_SPEED = 5.0f;
constexpr f32 NEAR_PLANE = 0.1f;
constexpr f32 FAR_PLANE = 1000.0f;

Camera::Camera(vec3 position, f32 fov) 
: m_position(position), m_fov(fov) {
}

void Camera::update(f32 dt) {
	PROFILE_FUNC();

	handle_mouse_movement();
	handle_movement(dt);
}

mat4 Camera::get_matrix(f32 aspect_ratio) const {
	PROFILE_FUNC();

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
	PROFILE_FUNC();

	vec3 movement = vec3(0.0f);

	vec3 forward = get_forward_direction();
	forward.y = 0.0f;
	forward = glm::normalize(forward);

	const vec3 right = get_right_direction();

	const Input &input = Input::get_instance();
	if(input.is_key_pressed(GLFW_KEY_A))				movement -= right;
	if(input.is_key_pressed(GLFW_KEY_D))				movement += right;
	if(input.is_key_pressed(GLFW_KEY_W))				movement += forward;
	if(input.is_key_pressed(GLFW_KEY_S))				movement -= forward;
	if(input.is_key_pressed(GLFW_KEY_SPACE))			movement += vec3(0.0f, 1.0f, 0.0f);
	if(input.is_key_pressed(GLFW_KEY_LEFT_SHIFT))		movement -= vec3(0.0f, 1.0f, 0.0f);

	if(movement.x == 0.0f && movement.y == 0.0f && movement.z == 0.0f) {
		return;
	}

	movement = glm::normalize(movement);
	m_position += movement * MOVE_SPEED * dt;
}

void Camera::handle_mouse_movement() {
	PROFILE_FUNC();

	Input &input = Input::get_instance();

	if(input.get_mouse_mode() != GLFW_CURSOR_DISABLED) {
		return;
	}

	const vec2 mouse_delta = input.get_mouse_delta();
	m_yaw += mouse_delta.x * SENSITIVITY;
	m_pitch -= mouse_delta.y * SENSITIVITY;

	m_pitch = glm::clamp(m_pitch, -glm::half_pi<f32>() + glm::radians(1.0f), glm::half_pi<f32>() - glm::radians(1.0f));
	m_yaw = glm::mod(m_yaw, glm::two_pi<f32>());
}
