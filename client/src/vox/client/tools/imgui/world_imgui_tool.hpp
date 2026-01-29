#pragma once

#include "vox/client/graphics/renderers/world_renderer.hpp"
#include "vox/client/world/client_world.hpp"

class WorldImGuiTool {
public:
	void render(ClientWorld &world, std::optional<std::reference_wrapper<WorldRenderer>> world_renderer = std::nullopt);
};
