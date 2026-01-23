#pragma once

#include "vox/common/world/world.hpp"

class GraphicsImGuiTool {
public:
    void render(std::optional<std::reference_wrapper<World>> world = std::nullopt);
};