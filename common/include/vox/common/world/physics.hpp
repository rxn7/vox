#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/common/world/i_world.hpp"

namespace Physics {
	struct RaycastResult {
		BlockPosition hit_block_position;
		ivec3 previous_grid_position;
		f32 distance = 0.0f;
	};

	std::optional<RaycastResult> raycast(const IWorld &world, vec3 start, vec3 dir, f32 max_distance);
}
