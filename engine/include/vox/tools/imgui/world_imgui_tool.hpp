#pragma once

#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/world/world.hpp"

class WorldImGuiTool {
public:
    void render(World &world, std::optional<std::reference_wrapper<WorldRenderer>> world_renderer = std::nullopt);
};