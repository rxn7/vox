#pragma once

#include "vox/client/graphics/backend/mesh_base.hpp"

template<typename Vertex>
class DynamicMesh : public MeshBase<Vertex> {
public:
	DynamicMesh(u32 initial_vertices_cap = 0, u32 initial_elements_cap = 0, GLenum draw_mode = GL_TRIANGLES) : MeshBase<Vertex>(draw_mode) {
		if(initial_vertices_cap > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, MeshBase<Vertex>::m_vbo);
			resize_vbo(initial_vertices_cap, nullptr);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		if(initial_elements_cap > 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshBase<Vertex>::m_ebo);
			resize_ebo(initial_elements_cap, nullptr);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	void set_data(std::span<const Vertex> vertices, std::span<const u16> elements) {
		PROFILE_FUNC();

		glBindVertexArray(MeshBase<Vertex>::m_vao);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, MeshBase<Vertex>::m_vbo);
		if(vertices.size() > m_vertices_cap) {
			resize_vbo(vertices.size(), vertices.data());
		} else {
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
		}

		// Elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshBase<Vertex>::m_ebo);
		if(elements.size() > m_elements_cap) {
			resize_ebo(elements.size(), elements.data());
		} else {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, elements.size() * sizeof(u16), elements.data());
		}

		MeshBase<Vertex>::m_element_count = elements.size();

		Vertex::setup_attributes();

		glBindVertexArray(0);
	}

private:
	void resize_vbo(u32 count, const Vertex *vertices) {
		m_vertices_cap = count;
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
	}

	void resize_ebo(u32 count, const u16 *elements) {
		m_elements_cap = count;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u16), elements, GL_DYNAMIC_DRAW);
	}

private:
	u32 m_vertices_cap = 0;
	u32 m_elements_cap = 0;
};
