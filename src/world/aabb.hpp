#pragma once

struct AABB {
	vec3 min;
	vec3 max;

	constexpr bool overlap(const AABB &other) const {
		return (
			min.x < other.max.x && max.x > other.min.x &&
			min.y < other.max.y && max.y > other.min.y &&
			min.z < other.max.z && max.z > other.min.z
		);
	}
};
