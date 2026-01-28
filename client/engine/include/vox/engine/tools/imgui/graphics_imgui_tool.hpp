#pragma once

#include "vox/engine/world/client_world.hpp"

class GraphicsImGuiTool {
public:
	void render(std::optional<std::reference_wrapper<ClientWorld>> world = std::nullopt);
};
