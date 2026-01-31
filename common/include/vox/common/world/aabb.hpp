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

	constexpr vec3 size() const {
		return (max - min);
	}

	constexpr vec3 half_size() const {
		return size() * 0.5f;
	}

	constexpr vec3 center() const {
		return min + half_size();
	}
};
