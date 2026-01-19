#include "mesh.hpp"

Mesh::Mesh() { }

Mesh::Mesh(std::span<const Vertex> vertices, std::span<const u16> elements) {
	set_data(vertices, elements);
}

Mesh::~Mesh() {
	if(!m_created) return;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void Mesh::draw() const {
	PROFILE_FUNC();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_element_count, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Mesh::set_data(std::span<const Vertex> vertices, std::span<const u16> elements) {
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

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	// UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	m_created = true;

	glBindVertexArray(0);
}
