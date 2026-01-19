#pragma once

struct Vertex {
	Vertex(vec3 position, vec2 uv) : position(position), uv(uv) {}

	vec3 position;
	vec2 uv;
};
