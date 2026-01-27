#pragma once

#include "vox/common/resources/texture_id.hpp"
#include "vox/common/world/block_face_id.hpp"

struct BlockTextures {
	constexpr BlockTextures() = default;
	constexpr BlockTextures(TextureID top, TextureID side, TextureID bottom) : right_texture(side), left_texture(side), top_texture(top), bottom_texture(bottom), front_texture(side), back_texture(side) {}

	union {
		struct {
			TextureID right_texture;
			TextureID left_texture; 
			TextureID top_texture;
			TextureID bottom_texture; 
			TextureID front_texture; 
			TextureID back_texture;
		};

		TextureID textures[6];
	};
};

class BlockType {
public:
	constexpr BlockType() : m_name("INVALID"), m_is_solid(false), m_is_transparent(true) { }

	constexpr BlockType(std::string_view name) : m_name(name), m_is_solid(false), m_is_transparent(true) { }

	constexpr BlockType(std::string_view name, TextureID texture, bool is_solid = true, bool is_transparent = false) 
	: m_name(name), m_is_solid(is_solid), m_is_transparent(is_transparent) {
		for(i32 i=0; i<6; ++i) {
			m_textures.textures[i] = texture;
		}
	}

	constexpr BlockType(std::string_view name, BlockTextures textures, bool is_solid = true, bool is_transparent = false)
	: m_name(name), m_is_solid(is_solid), m_is_transparent(is_transparent), m_textures(textures) {
	}

	TextureID get_texture_id(BlockFaceID face_id) const { 
		PROFILE_FUNC();
		
		return m_textures.textures[(u8)face_id]; 
	}

	std::string_view get_name() const { 
		return m_name; 
	}

	bool is_solid() const { 
		return m_is_solid; 
	}

	bool is_transparent() const { 
		return m_is_transparent; 
	}

	const BlockTextures &get_textures() const { 
		return m_textures; 
	}

private:
	std::string_view m_name;
	bool m_is_solid = false;
	bool m_is_transparent = false;
	BlockTextures m_textures;
};
