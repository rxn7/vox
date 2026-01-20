#pragma once

struct Vertex {
	constexpr Vertex(vec3 position, vec2 uv) : position(position), uv(uv) {}

	vec3 position;
	vec2 uv;
};
