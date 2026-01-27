#pragma once

struct Default3DVertex {
	constexpr Default3DVertex(vec3 position, vec2 uv) : position(position), uv(uv) {}

	inline static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Default3DVertex), (void*)offsetof(Default3DVertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Default3DVertex), (void*)offsetof(Default3DVertex, uv));
	}

	vec3 position;
	vec2 uv;
};

struct Default2DVertex {
	constexpr Default2DVertex(vec2 position, vec2 uv) : position(position), uv(uv) {}

	inline static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Default2DVertex), (void*)offsetof(Default2DVertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Default2DVertex), (void*)offsetof(Default2DVertex, uv));
	}

	vec2 position;
	vec2 uv;
};
