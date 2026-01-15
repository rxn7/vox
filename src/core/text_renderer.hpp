#pragma once

#include "singleton.hpp"
#include "shader.hpp"

class TextRenderer {
SINGLETON_CLASS(TextRenderer);
public:

	TextRenderer();
	~TextRenderer();

	void init();
	void update_screen_size(const vec2 &size);
	void render_text(std::string_view text, const vec2 &position, f32 size = 16.0f);

	[[nodiscard]] static constexpr u8 get_font_index(char c) {
		if(c < 32 || c > 126) [[unlikely]] {
			return 31; // '?'
		}

		return c - 32;
	}

private:
	bool m_initialized = false;
	Shader m_shader;
	u32 m_vao;
	u32 m_font_ssbo;
	u32 m_text_ssbo;

	std::vector<u8> m_staging_buffer;
};
