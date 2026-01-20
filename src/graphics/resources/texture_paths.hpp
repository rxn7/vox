#pragma once

#include "graphics/resources/texture_id.hpp"

namespace TexturePaths {
    extern const std::string_view &get(TextureID id);
    extern std::span<const std::string_view> get_all();
};
