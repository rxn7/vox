#pragma once

#include "vox/client/graphics/backend/static_mesh.hpp"
#include "vox/client/graphics/backend/vertex.hpp"
#include "vox/client/graphics/backend/shader.hpp"

class ModelPart {
public:
	ModelPart() : m_mesh(GL_TRIANGLES) {}

	void setup(std::span<const Default3DVertex> vertices, std::span<const u16> indices, vec3 local_position = vec3(0), vec3 pivot = vec3(0), vec3 rotation = vec3(0));
	void render(Shader &shader, const mat4 &parent_model_matrix);

public:
	vec3 m_local_position;
	vec3 m_pivot;
	vec3 m_rotation;

private:
	StaticMesh<Default3DVertex> m_mesh;
};
