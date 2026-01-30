#include "vox/client/graphics/renderers/debug_renderer.hpp"

constexpr u32 INITIAL_VERTEX_COUNT = 1024;
constexpr u32 INITIAL_ELEMENT_COUNT = 2048;

DebugRenderer::DebugRenderer() 
: m_mesh(INITIAL_VERTEX_COUNT, INITIAL_ELEMENT_COUNT, GL_LINES), 
m_shader(b::embed<"shaders/debug-vert.glsl">().data(), b::embed<"shaders/debug-frag.glsl">().data()) {
	m_vertices.reserve(INITIAL_VERTEX_COUNT);
	m_elements.reserve(INITIAL_ELEMENT_COUNT);

	f32 line_width_range[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, line_width_range);

	std::println("GL_LINES range: ({:.3f} - {:.3f}", line_width_range[0], line_width_range[1]);
}

void DebugRenderer::render(const mat4 &camera_matrix) {
	PROFILE_FUNC();
	m_camera_matrix = camera_matrix;

	m_shader.bind();
	m_shader.set_uniform_mat4("u_camera_matrix", camera_matrix);

	m_mesh.set_data(m_vertices, m_elements);

	glDisable(GL_DEPTH_TEST);
	glLineWidth(3.0f);
	m_mesh.render();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);

	m_vertices.clear();
	m_elements.clear();
}

void DebugRenderer::draw_aabb(const AABB &aabb, vec3 color) {
	PROFILE_FUNC();

	const u16 base_index = static_cast<u16>(m_vertices.size());

    const vec3 &min = aabb.min;
    const vec3 &max = aabb.max;

    // bottom face
    m_vertices.emplace_back(vec3(min.x, min.y, min.z), color); // 0
    m_vertices.emplace_back(vec3(max.x, min.y, min.z), color); // 1
    m_vertices.emplace_back(vec3(max.x, min.y, max.z), color); // 2
    m_vertices.emplace_back(vec3(min.x, min.y, max.z), color); // 3

	// top face
    m_vertices.emplace_back(vec3(min.x, max.y, min.z), color); // 4
    m_vertices.emplace_back(vec3(max.x, max.y, min.z), color); // 5
    m_vertices.emplace_back(vec3(max.x, max.y, max.z), color); // 6
    m_vertices.emplace_back(vec3(min.x, max.y, max.z), color); // 7
    
    // bottom square
    m_elements.insert(m_elements.end(), {
        (u16)(base_index + 0), (u16)(base_index + 1),
        (u16)(base_index + 1), (u16)(base_index + 2),
        (u16)(base_index + 2), (u16)(base_index + 3),
        (u16)(base_index + 3), (u16)(base_index + 0)
    });

	// top square
    m_elements.insert(m_elements.end(), {
        (u16)(base_index + 4), (u16)(base_index + 5),
        (u16)(base_index + 5), (u16)(base_index + 6),
        (u16)(base_index + 6), (u16)(base_index + 7),
        (u16)(base_index + 7), (u16)(base_index + 4)
    });

	// vertical lines
    m_elements.insert(m_elements.end(), {
        (u16)(base_index + 0), (u16)(base_index + 4),
        (u16)(base_index + 1), (u16)(base_index + 5),
        (u16)(base_index + 2), (u16)(base_index + 6),
        (u16)(base_index + 3), (u16)(base_index + 7)
    });
}

void DebugRenderer::draw_line(vec3 start, vec3 end, vec3 color) {
	PROFILE_FUNC();

	const u16 base_idx = static_cast<u16>(m_vertices.size());

	m_vertices.emplace_back(start, color);
	m_vertices.emplace_back(end, color);

	m_elements.emplace_back(base_idx);
	m_elements.emplace_back(base_idx + 1);
}
