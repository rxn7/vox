#pragma once

struct Default3DVertex {
	constexpr Default3DVertex(vec3 position, vec2 uv) : m_position(position), m_uv(uv) {}

	inline static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Default3DVertex), (void*)offsetof(Default3DVertex, m_position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Default3DVertex), (void*)offsetof(Default3DVertex, m_uv));
	}

	vec3 m_position;
	vec2 m_uv;
};

struct Default2DVertex {
	constexpr Default2DVertex(vec2 position, vec2 uv) : m_position(position), m_uv(uv) {}

	inline static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Default2DVertex), (void*)offsetof(Default2DVertex, m_position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Default2DVertex), (void*)offsetof(Default2DVertex, m_uv));
	}

	vec2 m_position;
	vec2 m_uv;
};
