#include "graphics/renderers/text_renderer.hpp"
#include "graphics/resources/font_data.hpp"

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
	m_shader.load(b::embed<"shaders/ui/text-vert.glsl">().str(), b::embed<"shaders/ui/text-frag.glsl">().str());

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

void TextRenderer::render_text(std::string_view text, vec2 position, f32 size) {
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

	m_shader.set_uniform_f32("u_font_size", size);
	m_shader.set_uniform_vec2("u_position", position);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, text.size() );

	glBindVertexArray(0);
}

void TextRenderer::handle_window_resize(vec2 size) {
	PROFILE_FUNC();
	m_shader.set_uniform_vec2("u_screen_size", size);
}
