#include "vox/client/graphics/model_part.hpp"

void ModelPart::setup(std::span<const Default3DVertex> vertices, std::span<const u16> indices, vec3 local_position, vec3 pivot, vec3 rotation) {
	m_local_position = local_position;
	m_pivot = pivot;
	m_rotation = rotation;

	m_mesh.set_data(vertices, indices);
}

void ModelPart::render(Shader &shader, const mat4 &parent_model_matrix) {
	mat4 model_matrix = parent_model_matrix;
	model_matrix = glm::translate(model_matrix, m_local_position);

	model_matrix = glm::translate(model_matrix, m_pivot);
	model_matrix = glm::rotate(model_matrix, m_rotation.z, FRONT);
	model_matrix = glm::rotate(model_matrix, m_rotation.y, UP);
	model_matrix = glm::rotate(model_matrix, m_rotation.x, RIGHT);
	model_matrix = glm::translate(model_matrix, -m_pivot);

	shader.bind();
	shader.set_uniform_mat4("u_model_matrix", model_matrix);
	m_mesh.render();
}
