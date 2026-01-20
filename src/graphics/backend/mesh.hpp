#pragma once

#include "graphics/backend/vertex.hpp"

class Mesh {
public:
	Mesh();
	Mesh(std::span<const Vertex> vertices, std::span<const u16> indices);
	~Mesh();

	void set_data(std::span<const Vertex> vertices, std::span<const u16> indices);
	void render() const;
	
private:
	bool m_created = false;

	u32 m_element_count;
	u32 m_vao;
	u32 m_vbo;
	u32 m_ebo;
};
