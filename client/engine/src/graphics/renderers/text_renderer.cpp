#include "vox/engine/graphics/renderers/text_renderer.hpp"
#include "vox/engine/graphics/resources/font_data.hpp"

#include <battery/embed.hpp>

constexpr u32 STAGING_BUFFER_SIZE = 2048;

TextRenderer::TextRenderer() {
}

TextRenderer::~TextRenderer() {
	if(!m_initialized) return;

	glDeleteBuffers(1, &m_font_ssbo);
	glDeleteBuffers(1, &m_text_ssbo);
	glDeleteVertexArrays(1, &m_vao);
}

void TextRenderer::init() {
	PROFILE_FUNC();

	m_shader.load(b::embed<"shaders/text-vert.glsl">().str(), b::embed<"shaders/text-frag.glsl">().str());

	m_staging_buffer.reserve(STAGING_BUFFER_SIZE);

	glGenVertexArrays(1, &m_vao);

	// Static font data
	glGenBuffers(1, &m_font_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_font_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(FONT_DATA.bytes), FONT_DATA.bytes.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_font_ssbo);

	// Dynamic text data
	glGenBuffers(1, &m_text_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_text_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, STAGING_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_text_ssbo);

	m_initialized = true;
}


void TextRenderer::update_2d(vec2 screen_size) {
	PROFILE_FUNC();

	m_projection_view_matrix_2d = glm::ortho(0.0f, screen_size.x, 0.0f, screen_size.y, -1.0f, 1.0f);
}

void TextRenderer::update_3d(const mat4 &proj_matrix, const mat4 &view_matrix) {
	PROFILE_FUNC();
	m_proj_matrix_3d = proj_matrix;
	m_view_matrix_3d = view_matrix;
}

void TextRenderer::render_text(std::string_view text, mat4 mvp, f32 size, TextHorizontalAlign horizontal_align, TextVerticalAlign vertical_align, vec4 color) {
	PROFILE_FUNC();

	if(text.empty()) [[unlikely]] {
		return;
	}

	if(text.size() > STAGING_BUFFER_SIZE) [[unlikely]] {
		std::println("Text too long ({} > {}) : {}", text.size(), STAGING_BUFFER_SIZE, text);
	}

	m_staging_buffer.clear();
	for(char c : text) {
		m_staging_buffer.push_back(get_font_index(c));
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_text_ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, text.size() * sizeof(u8), m_staging_buffer.data());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_text_ssbo);

	glBindVertexArray(m_vao);

	m_shader.set_uniform_mat4("u_mvp", mvp);
	m_shader.set_uniform_f32("u_font_size", size);
	m_shader.set_uniform_i32("u_text_length", text.size());
	m_shader.set_uniform_i32("u_horizontal_align", static_cast<i32>(horizontal_align));
	m_shader.set_uniform_i32("u_vertical_align", static_cast<i32>(vertical_align));
	m_shader.set_uniform_vec4("u_color", color);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, text.size() );

	PROFILE_FUNC();
	glBindVertexArray(0);
}

void TextRenderer::render_text_2d(const TextRenderCommand2D &cmd) {
	PROFILE_FUNC();

	const mat4 model = glm::translate(mat4(1.0f), vec3(cmd.position, 0));
	render_text(cmd.text, m_projection_view_matrix_2d * model, cmd.size, cmd.horizontal_align, cmd.vertical_align, cmd.color);
}

void TextRenderer::render_text_3d(const TextRenderCommand3D &cmd) {
	PROFILE_FUNC();

	mat4 model = glm::translate(mat4(1.0f), cmd.position);

	if(cmd.billboard) {
		glDisable(GL_DEPTH_TEST);

		model[0][0] = m_view_matrix_3d[0][0]; 
		model[0][1] = m_view_matrix_3d[1][0]; 
		model[0][2] = m_view_matrix_3d[2][0];

		model[1][0] = m_view_matrix_3d[0][1]; 
		model[1][1] = m_view_matrix_3d[1][1]; 
		model[1][2] = m_view_matrix_3d[2][1];

		model[2][0] = m_view_matrix_3d[0][2]; 
		model[2][1] = m_view_matrix_3d[1][2]; 
		model[2][2] = m_view_matrix_3d[2][2];
	}

	render_text(cmd.text, m_proj_matrix_3d * m_view_matrix_3d * model, cmd.size, cmd.horizontal_align, cmd.vertical_align, cmd.color);

	glEnable(GL_DEPTH_TEST);
}
