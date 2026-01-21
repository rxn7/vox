#include "vox/graphics/resources/texture_paths.hpp"

#define TEXTURE_PATH(name) "assets/textures/" name

// NOTE: its done that way to make explicit assignment to a specific BlockID.
inline constexpr auto TEXTURE_PATHS = []() {
    std::array<std::string_view, static_cast<sz>(TextureID::SIZE)> paths{};

    for(auto &p : paths) 
		p = TEXTURE_PATH("error.png");

    #define SET_TEXTURE(id, name) paths[static_cast<sz>(TextureID::id)] = TEXTURE_PATH(name)

    SET_TEXTURE(Dirt, "dirt.png");
    SET_TEXTURE(GrassTop, "grass_top.png");
    SET_TEXTURE(GrassSide, "grass_side.png");
    SET_TEXTURE(Stone, "stone.png");

    return paths;
}();

const std::string_view &TexturePaths::get(TextureID id) {
	return TEXTURE_PATHS[static_cast<sz>(id)];
}

std::span<const std::string_view> TexturePaths::get_all() {
    return TEXTURE_PATHS;
}
