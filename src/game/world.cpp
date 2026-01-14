#include "world.hpp"
#include "core/input.hpp"

const std::array<Vertex, 3> VERTICES = {
	Vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(0,0)),
	Vertex(vec3(0.5f, -0.5f, 0.0f), vec2(1,0)),
	Vertex(vec3(0.0f, 0.5f, 0.0f), vec2(0.5f, 1))
};

const std::array ELEMENTS = {
	u16(0), u16(1), u16(2)
};

World::World() 
: m_camera(vec3(0, 0, 1.0f), 80.0f) {
	m_shader.load(b::embed<"shaders/default-vert.glsl">().str(), b::embed<"shaders/default-frag.glsl">().str());
	m_mesh.set_data(std::span(VERTICES), std::span(ELEMENTS));
}

World::~World() {
}

void World::update(f32 delta_time) {
	m_camera.update(delta_time);
}

void World::render(f32 aspect_ratio) {
	const mat4 view_matrix = m_camera.get_matrix(aspect_ratio);
	m_shader.set_uniform_mat4("u_cameraMatrix", view_matrix);

	m_shader.bind();
	m_mesh.draw();
}
