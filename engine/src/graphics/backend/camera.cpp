#include "vox/graphics/backend/camera.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

constexpr f32 NEAR_PLANE = 0.1f;
constexpr f32 FAR_PLANE = 1000.0f;

Camera::Camera(vec3 position, f32 fov) 
: m_position(position), m_fov(fov) {
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
