#pragma once

#include "shader.hpp"

class TextRenderer {
public:
	TextRenderer();
	~TextRenderer();

	inline static TextRenderer &get_instance() {
		if(!sp_instance) [[unlikely]] {
			sp_instance = std::make_unique<TextRenderer>();
		}

		return *sp_instance.get();
	}

	inline static void destroy_instance() { sp_instance.reset(); }

	void init();
	void update_screen_size(const vec2 &size);
	void render_text(std::string_view text, const vec2 &position, f32 size = 16.0f);

	static constexpr u8 get_font_index(char c) {
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

	static std::unique_ptr<TextRenderer> sp_instance;
};
