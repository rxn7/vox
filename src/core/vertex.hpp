#pragma once

struct Vertex {
	vec3 position;
	// vec3 normal;
	vec2 uv;

	Vertex(vec3 position, vec2 uv) : position(position), uv(uv) {}
};
