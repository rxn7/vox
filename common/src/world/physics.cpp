#include "vox/common/world/physics.hpp"

std::optional<Physics::RaycastResult> Physics::raycast(const IWorld &world, vec3 start, vec3 dir, f32 max_distance) {
	PROFILE_FUNC();

	ivec3 grid_position = ivec3(glm::floor(start));
	const ivec3 step_dir = ivec3(glm::sign(dir));

	const vec3 delta_dist = vec3(
		std::abs(dir.x) < 1e-4f ? 1e30f : std::abs(1.0f / dir.x),
		std::abs(dir.y) < 1e-4f ? 1e30f : std::abs(1.0f / dir.y),
		std::abs(dir.z) < 1e-4f ? 1e30f : std::abs(1.0f / dir.z)
	);

	vec3 side_dist = vec3(
		(dir.x < 0) ? (start.x - grid_position.x) * delta_dist.x : (grid_position.x + 1.0f - start.x) * delta_dist.x,
		(dir.y < 0) ? (start.y - grid_position.y) * delta_dist.y : (grid_position.y + 1.0f - start.y) * delta_dist.y,
		(dir.z < 0) ? (start.z - grid_position.z) * delta_dist.z : (grid_position.z + 1.0f - start.z) * delta_dist.z
	);

	ivec3 last_grid_position = grid_position;
	f32 distance_traveled = 0.0f;
	while(distance_traveled < max_distance) {
		const vec3 test_position = vec3(grid_position) + 0.5f;

		const BlockPosition block_position = BlockPosition(vec3(test_position));
		const BlockID block_id = world.get_block(block_position);

		if(block_id != BlockID::Air) {
			const BlockType &block_type = BlockRegistry::get(block_id);

			if(block_type.is_solid()) {
				return RaycastResult {
					.hit_block_position = block_position,
					.previous_grid_position = last_grid_position,
					.distance = distance_traveled
				};
			}
		}

		last_grid_position = grid_position;
		i32 axis = 0;
		if(side_dist[1] < side_dist[0])
			axis = 1;
		if(side_dist[2] < side_dist[axis])
			axis = 2;

		distance_traveled = side_dist[axis];
		side_dist[axis] += delta_dist[axis];
		grid_position[axis] += step_dir[axis];
	}

	return std::nullopt;
}

std::optional<Physics::CollisionResult> Physics::check_collision(IWorld &world, const AABB &aabb) {
	PROFILE_FUNC();

	const i32 min_x = static_cast<i32>(glm::floor(aabb.min.x));
	const i32 max_x = static_cast<i32>(glm::floor(aabb.max.x));

	const i32 min_y = static_cast<i32>(glm::floor(aabb.min.y));
	const i32 max_y = static_cast<i32>(glm::floor(aabb.max.y));

	const i32 min_z = static_cast<i32>(glm::floor(aabb.min.z));
	const i32 max_z = static_cast<i32>(glm::floor(aabb.max.z));

	for(i32 x = min_x; x <= max_x; ++x) {
		for(i32 y = min_y; y <= max_y; ++y) {
			for(i32 z = min_z; z <= max_z; ++z) {
				const vec3 block_center = vec3(x, y, z) + 0.5f;
				const BlockPosition check_position = BlockPosition(block_center);
				const BlockID block_id = world.get_block(check_position);
				const BlockType &block_type = BlockRegistry::get(block_id);

				if(!block_type.is_solid()) {
					continue;
				}

				const vec3 block_half_size = vec3(0.5f);
				const vec3 aabb_center = aabb.center();

				const vec3 delta = aabb_center - block_center;
				const vec3 overlap = (aabb.half_size() + block_half_size) - glm::abs(delta);

				const vec3 normal = Physics::calculate_normal(overlap, delta);

				return CollisionResult {
					.block_type = block_type,
					.block_center = block_center,
					.normal = normal,
				};
			}
		}
	}

	return std::nullopt;
}

vec3 Physics::calculate_normal(vec3 overlap, vec3 delta) {
	if(overlap.x < overlap.y && overlap.x < overlap.z) {
		return vec3(glm::sign(delta.x), 0.0f, 0.0f);
	}

	if(overlap.y < overlap.x && overlap.y < overlap.z) {
		return vec3(0.0f, glm::sign(delta.y), 0.0f);
	}

	return vec3(0.0f, 0.0f, glm::sign(delta.z));
}
