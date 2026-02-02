#include "vox/client/game.hpp"
#include "vox/client/core/engine.hpp"
#include "vox/common/helper/math_utils.hpp"
#include "vox/client/player/player_model.hpp"
#include "vox/client/graphics/mesh_utils.hpp"

PlayerModel::PlayerModel() {
}

void PlayerModel::update(vec3 position, vec3 velocity, f32 pitch, f32 yaw) {
	const vec3 horizontal_velocity = vec3(velocity.x, 0.0f, velocity.z);
	const f32 speed = glm::length(horizontal_velocity);

	handle_body_yaw(horizontal_velocity, speed, yaw);
	handle_head_rotation(pitch, yaw);
	handle_walk_animation(speed);
}

void PlayerModel::render(vec3 position) {
	mat4 model_matrix = glm::translate(position);
	model_matrix = glm::rotate(model_matrix, m_body_yaw, DOWN);

	Shader &shader = Game::get_instance()->get_default_3d_shader();

	m_torso.render(shader, model_matrix);
	m_left_arm.render(shader, model_matrix);
	m_right_arm.render(shader, model_matrix);
	m_right_leg.render(shader, model_matrix);
	m_left_leg.render(shader, model_matrix);
	m_head.render(shader, model_matrix);
}

void PlayerModel::build_geometry() {
	std::vector<Default3DVertex> vertices;
	vertices.reserve(4 * 6); // 4 vertices per face, 6 faces per cube

	std::vector<u16> indices;
	indices.reserve(6 * 6); // 6 elements per face, 6 faces per cube

	MeshUtils::create_box(vertices, indices, {0.5f, 0.75f, 0.25f});
	m_torso.setup(vertices, indices, vec3(0, 1.1f, 0));

	vertices.clear();
	indices.clear();

	MeshUtils::create_box(vertices, indices, {0.5, 0.5, 0.5});
	m_head.setup(vertices, indices, vec3(0, 1.725f, 0), vec3(0.0f, -0.25f, 0.0f));

	vertices.clear();
	indices.clear();

	// limb
	MeshUtils::create_box(vertices, indices, {0.25f, 0.75f, 0.25f});

	const vec3 limb_pivot = vec3(0.0f, 0.15f, 0.0f);

	m_left_leg.setup(vertices, indices, vec3(-0.125f, 0.35f, 0), limb_pivot);
	m_right_leg.setup(vertices, indices, vec3(0.125f, 0.35f, 0), limb_pivot);
	m_left_arm.setup(vertices, indices, vec3(-0.375f, 1.1f, 0), limb_pivot);
	m_right_arm.setup(vertices, indices, vec3(0.375f, 1.1f, 0), limb_pivot);
}

void PlayerModel::handle_body_yaw(vec3 vel, f32 speed, f32 camera_yaw) {
	const f32 BODY_YAW_SMOOTH_SPEED = 10.0f;
	const f32 dt = Engine::get_instance().get_delta_time();

	m_body_yaw = MathUtils::lerp_angle(m_body_yaw, camera_yaw, glm::clamp(dt * BODY_YAW_SMOOTH_SPEED, 0.0f, 1.0f));
}

void PlayerModel::handle_head_rotation(f32 pitch, f32 yaw) {
	const f32 HEAD_YAW_SMOOTH_SPEED = 15.0f;
	const f32 dt = Engine::get_instance().get_delta_time();

	const f32 head_local_yaw = MathUtils::angle_diff(yaw, m_body_yaw);
	m_head.m_rotation.x = MathUtils::lerp_angle(m_head.m_rotation.x, -pitch, glm::clamp(dt * HEAD_YAW_SMOOTH_SPEED, 0.0f, 1.0f));
	m_head.m_rotation.y = MathUtils::lerp_angle(m_head.m_rotation.y, head_local_yaw, glm::clamp(dt * HEAD_YAW_SMOOTH_SPEED, 0.0f, 1.0f));
}

void PlayerModel::handle_walk_animation(f32 speed) {
	const f32 dt = Engine::get_instance().get_delta_time();

	f32 target_swing = 0.0f;
	if(speed > 0.0f) {
		constexpr f32 SWING_FREQUENCY = 5.0f;
		constexpr f32 SWING_AMPLITUDE = 0.8f;

		m_walk_time += dt * speed * SWING_FREQUENCY;

		const f32 speed_factor = speed / PLAYER_WALK_SPEED;

		target_swing = glm::sin(m_walk_time) * SWING_AMPLITUDE * speed_factor;
	}

	constexpr f32 SWING_SMOOTH_SPEED = 15.0f;
	m_swing = glm::lerp(m_swing, target_swing, dt * SWING_SMOOTH_SPEED);

	m_right_arm.m_rotation.x = m_swing;
	m_left_leg.m_rotation.x = m_swing;

	m_left_arm.m_rotation.x = -m_swing;
	m_right_leg.m_rotation.x = -m_swing;
}
