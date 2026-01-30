#pragma once

template<typename Vertex>
class MeshBase {
public:
	MeshBase(const MeshBase &) = delete;
	MeshBase &operator=(const MeshBase &) = delete;

	virtual ~MeshBase() {
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ebo);
	}

	void render() const {
		PROFILE_FUNC();

		if(m_element_count == 0) [[unlikely]] {
			return;
		}

		glBindVertexArray(m_vao);
		glDrawElements(m_draw_mode, m_element_count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}

protected:
	MeshBase(GLenum draw_mode = GL_TRIANGLES) : m_draw_mode(draw_mode) {
		glCreateVertexArrays(1, &m_vao);
		glCreateBuffers(1, &m_vbo);
		glCreateBuffers(1, &m_ebo);
	}

protected:
	GLenum m_draw_mode = GL_TRIANGLES;

	u32 m_element_count;
	u32 m_vao;
	u32 m_vbo;
	u32 m_ebo;
};
