#pragma once

#include "vox/engine/graphics/backend/shader.hpp"

enum class TextHorizontalAlign : u8 {
	Left,
	Center,
	Right
};

enum class TextVerticalAlign : u8 {
	Top,
	Middle,
	Bottom
};

struct TextRenderCommandBase {
	std::string text;
	f32 size = 16.0f;
	vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	TextHorizontalAlign horizontal_align = TextHorizontalAlign::Left;
	TextVerticalAlign vertical_align = TextVerticalAlign::Bottom;
};

struct TextRenderCommand2D : public TextRenderCommandBase {
	vec2 position;
};

struct TextRenderCommand3D : public TextRenderCommandBase {
	vec3 position;
	bool billboard = false;
};

class TextRenderer {
public:
	TextRenderer();
	~TextRenderer();

	void init();

	void update_2d(vec2 screen_size);
	void update_3d(const mat4 &proj_matrix, const mat4 &view_matrix);

	void render_text_2d(const TextRenderCommand2D &command);
	void render_text_3d(const TextRenderCommand3D &command);

	static constexpr u8 get_font_index(char c) {
		if(c < 32 || c > 126) [[unlikely]] {
			return 31; // '?'
		}

		return c - 32;
	}

private:
	void render_text(std::string_view text, mat4 mvp, f32 size, TextHorizontalAlign horizontal_align, TextVerticalAlign vertical_align, vec4 color);

private:
	bool m_initialized = false;
	Shader m_shader;
	u32 m_vao;
	u32 m_font_ssbo;
	u32 m_text_ssbo;

	mat4 m_projection_view_matrix_2d = mat4(1.0f);
	mat4 m_view_matrix_3d = mat4(1.0f);
	mat4 m_proj_matrix_3d = mat4(1.0f);

	std::vector<u8> m_staging_buffer;
};
