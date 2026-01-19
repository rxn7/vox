#pragma once

#include "core/texture_id.hpp"
#include "core/render/voxel.hpp"

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

struct BlockType {
	constexpr BlockType() : name("INVALID") { }

	constexpr BlockType(std::string_view name, TextureID texture, bool is_solid = true, bool is_transparent = false) 
	: name(name), is_solid(is_solid), is_transparent(is_transparent) {
		for(i32 i=0; i<6; ++i) {
			textures.textures[i] = texture;
		}
	}

	constexpr BlockType(std::string_view name, BlockTextures textures, bool is_solid = true, bool is_transparent = false)
	: name(name), is_solid(is_solid), is_transparent(is_transparent), textures(textures) {}

	constexpr TextureID get_texture_id(FaceID face_id) const { 
		return textures.textures[(u8)face_id]; 
	}

	std::string_view name;
	bool is_solid = false;
	bool is_transparent = false;
	BlockTextures textures;
};
