#pragma once

#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/common/world/world.hpp"

class WorldImGuiTool {
public:
    void render(World &world, std::optional<std::reference_wrapper<WorldRenderer>> world_renderer = std::nullopt);
};