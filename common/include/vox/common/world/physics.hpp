#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/common/world/i_world.hpp"
#include "vox/common/world/aabb.hpp"

enum Axis : u8 {
	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,
};

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

	std::optional<CollisionResult> check_collision(const IWorld &world, const AABB &aabb);

	// NOTE: this function changes the value of positiom, velocity and aabb references
	bool move_axis(const IWorld &world, Axis axis, f32 &position, f32 &velocity, AABB &aabb, f32 extent, f32 dt);

	vec3 calculate_normal(vec3 overlap, vec3 delta);
}
