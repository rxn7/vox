#pragma once

#include "vox/client/graphics/backend/mesh_base.hpp"

template<typename Vertex>
class StaticMesh : public MeshBase<Vertex> {
public:
	StaticMesh(GLenum draw_mode) : MeshBase<Vertex>(draw_mode) {
	}

	StaticMesh(std::span<const Vertex> vertices, std::span<const u16> elements, GLenum draw_mode = GL_TRIANGLES) : MeshBase<Vertex>(draw_mode) {
		set_data(vertices, elements);
	}

	void set_data(std::span<const Vertex> vertices, std::span<const u16> elements) {
		if(vertices.size() < 3 || elements.size() < 3) [[unlikely]] {
			MeshBase<Vertex>::m_element_count = 0;
			return;
		}

		glBindVertexArray(MeshBase<Vertex>::m_vao);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, MeshBase<Vertex>::m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshBase<Vertex>::m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(u16), elements.data(), GL_STATIC_DRAW);

		MeshBase<Vertex>::m_element_count = elements.size();

		Vertex::setup_attributes();

		glBindVertexArray(0);
	}
};
