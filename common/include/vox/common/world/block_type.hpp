#pragma once

#include "vox/common/resources/texture_id.hpp"
#include "vox/common/world/block_face_id.hpp"

struct BlockTextures {
	constexpr BlockTextures() = default;
	constexpr BlockTextures(TextureID top, TextureID side, TextureID bottom) : m_right_texture(side), m_left_texture(side), m_top_texture(top), m_bottom_texture(bottom), m_front_texture(side), m_back_texture(side) {}

	union {
		struct {
			TextureID m_right_texture;
			TextureID m_left_texture; 
			TextureID m_top_texture;
			TextureID m_bottom_texture; 
			TextureID m_front_texture; 
			TextureID m_back_texture;
		};

		TextureID m_textures[6];
	};
};

struct BlockType {
	constexpr BlockType() : m_name("INVALID"), m_is_solid(false), m_is_transparent(true) { }

	constexpr BlockType(std::string_view name, TextureID texture, bool is_solid = true, bool is_transparent = false) 
	: m_name(name), m_is_solid(is_solid), m_is_transparent(is_transparent) {
		for(i32 i=0; i<6; ++i) {
			m_textures.m_textures[i] = texture;
		}
	}

	constexpr BlockType(std::string_view name, BlockTextures textures, bool is_solid = true, bool is_transparent = false)
	: m_name(name), m_is_solid(is_solid), m_is_transparent(is_transparent), m_textures(textures) {
	}

	TextureID get_texture_id(BlockFaceID face_id) const { 
		PROFILE_FUNC();
		
		return m_textures.m_textures[(u8)face_id]; 
	}

	std::string_view m_name;
	bool m_is_solid = false;
	bool m_is_transparent = false;
	BlockTextures m_textures;
};
