#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/common/world/i_world.hpp"
#include "vox/common/world/aabb.hpp"

namespace Physics {
	struct RaycastResult {
		BlockPosition hit_block_position;
		ivec3 previous_grid_position;
		f32 distance = 0.0f;
	};

	struct CollisionResult {
		const BlockType &block_type;
		vec3 block_center;
		vec3 normal;
	};

	std::optional<RaycastResult> raycast(const IWorld &world, vec3 start, vec3 dir, f32 max_distance);

	std::optional<CollisionResult> check_collision(IWorld &world, const AABB &aabb);

	vec3 calculate_normal(vec3 overlap, vec3 delta);
}
