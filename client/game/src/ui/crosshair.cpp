#include "ui/crosshair.hpp"

#include <battery/embed.hpp>

Crosshair::Crosshair() {}

Crosshair::~Crosshair() {}

constexpr std::array<CrosshairVertex, 4> VERTICES = {
	CrosshairVertex(vec2(-0.5f, -0.5f)),
	CrosshairVertex(vec2(0.5f, -0.5f)),
	CrosshairVertex(vec2(0.5f, 0.5f)),
	CrosshairVertex(vec2(-0.5f, 0.5f))
};

constexpr std::array<u16, 6> INDICES = { 0, 1, 2, 2, 3, 0 };

void Crosshair::init() {
	PROFILE_FUNC();

	m_shader.load(b::embed<"shaders/ui/crosshair-vert.glsl">().str(), b::embed<"shaders/ui/crosshair-frag.glsl">().str());
	m_mesh.init(VERTICES, INDICES);
}

void Crosshair::render(vec2 window_size) {
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

    const vec2 scale = vec2(
		(m_size / window_size.x) * 2.0f,
		(m_size / window_size.y) * 2.0f
	);

	m_shader.bind();
	m_shader.set_uniform_f32("u_thickness", m_thickness);
	m_shader.set_uniform_vec2("u_scale", scale);
	m_mesh.render();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
