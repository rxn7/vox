#include "vox/engine/graphics/renderers/block_outline_renderer.hpp"

constexpr std::array<BlockOutlineVertex, 8> VERTICES = {
	BlockOutlineVertex({0.0f, 0.0f, 0.0f}),
	BlockOutlineVertex({1.0f, 0.0f, 0.0f}),
	BlockOutlineVertex({1.0f, 1.0f, 0.0f}),
	BlockOutlineVertex({0.0f, 1.0f, 0.0f}),
	BlockOutlineVertex({0.0f, 0.0f, 1.0f}),
	BlockOutlineVertex({1.0f, 0.0f, 1.0f}),
	BlockOutlineVertex({1.0f, 1.0f, 1.0f}),
	BlockOutlineVertex({0.0f, 1.0f, 1.0f})
};

constexpr std::array<u16, 36> INDICES = { 
	// Front Face (Z=0)
	0, 2, 1, 0, 3, 2,
	// Back Face (Z=1)
	5, 7, 4, 5, 6, 7,
	// Left Face (X=0)
	4, 3, 0, 4, 7, 3,
	// Right Face (X=1)
	1, 6, 5, 1, 2, 6,
	// Bottom Face (Y=0)
	4, 1, 5, 4, 0, 1,
	// Top Face (Y=1)
	3, 6, 2, 3, 7, 6
};

void BlockOutlineRenderer::init() {
	PROFILE_FUNC();

	m_shader.load(b::embed<"shaders/block-outline-vert.glsl">().str(), b::embed<"shaders/block-outline-frag.glsl">().str());
	m_mesh.init(VERTICES, INDICES);
}

void BlockOutlineRenderer::render(BlockPosition block_position, const mat4 &camera_matrix) {
	PROFILE_FUNC();
	PROFILE_FUNC();

	const vec3 global_position = vec3(block_position.local_position) + vec3(block_position.chunk_position.x, 0, block_position.chunk_position.y) * CHUNK_WIDTH;

	mat4 model = mat4(1.0f);
	model = glm::translate(model, global_position - vec3(0.005f));
	model = glm::scale(model, vec3(1.01f));

	m_shader.bind();

	m_shader.set_uniform_mat4("u_camera_matrix", camera_matrix);
	m_shader.set_uniform_mat4("u_model_matrix", model);
	m_shader.set_uniform_vec4("u_color", m_color);
	m_shader.set_uniform_f32("u_thickness", m_thickness);

	m_mesh.render();
}
