#include "vox/engine/graphics/backend/texture_array.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureArray::TextureArray() {
}

TextureArray::~TextureArray() {
	if(!m_initialized) {
		return;
	}

	glDeleteTextures(1, &m_texture_id);
}

bool TextureArray::load(std::span<const std::string_view> paths) {
	if(paths.size() == 0) [[unlikely]] {
		return false;
	}

	constexpr u32 TEXTURE_WIDTH = 16;
	constexpr u32 TEXTURE_HEIGHT = 16;

	const i32 texture_count = static_cast<i32>(paths.size());
	const i32 mip_levels = 4;

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texture_id);
	glTextureStorage3D(m_texture_id, mip_levels, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, texture_count);

	for(i32 i = 0; i < texture_count; i++) {
		const std::string_view path = paths[i];

		i32 width, height, channels;
		const u8 *data = stbi_load(path.data(), &width, &height, &channels, 4);

		if(data == nullptr) [[unlikely]] {
			std::println("Failed to load texture: {}", path);
			continue;
		}

		if(width != TEXTURE_WIDTH || height != TEXTURE_HEIGHT) [[unlikely]] {
			std::println("Texture dimensions must be {}x{}. {} is {}x{}.", TEXTURE_WIDTH, TEXTURE_HEIGHT, path, width, height);
			continue;
		}

		glTextureSubImage3D(m_texture_id, 0, 0, 0, i, TEXTURE_WIDTH, TEXTURE_HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free((void*)data);
	}

	glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateTextureMipmap(m_texture_id);

	m_initialized = true;

	return true;
}
