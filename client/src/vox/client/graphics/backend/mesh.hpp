#pragma once

template<typename Vertex>
class Mesh {
public:
	Mesh(GLenum draw_mode = GL_TRIANGLES) : m_draw_mode(draw_mode) {};

	~Mesh() {
		if(!m_created) {
			return;
		}

		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ebo);
	}

	void render() const {
		PROFILE_FUNC();

		glBindVertexArray(m_vao);
		glDrawElements(m_draw_mode, m_element_count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}

	void init(std::span<const Vertex> vertices, std::span<const u16> elements) {
		PROFILE_FUNC();
		
		assert(vertices.size() >= 3 && elements.size() >= 3);

		m_element_count = elements.size();

		glCreateVertexArrays(1, &m_vao);
		glCreateBuffers(1, &m_vbo);
		glCreateBuffers(1, &m_ebo);

		glBindVertexArray(m_vao);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(u16), elements.data(), GL_STATIC_DRAW);

		Vertex::setup_attributes();

		m_created = true;

		glBindVertexArray(0);
	}
	
private:
	bool m_created = false;

	GLenum m_draw_mode = GL_TRIANGLES;

	u32 m_element_count;
	u32 m_vao;
	u32 m_vbo;
	u32 m_ebo;
};
