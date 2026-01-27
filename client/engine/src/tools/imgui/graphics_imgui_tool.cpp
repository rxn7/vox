#include "vox/engine/tools/imgui/graphics_imgui_tool.hpp"
#include "vox/engine/graphics/graphics_settings.hpp"

void GraphicsImGuiTool::render(std::optional<std::reference_wrapper<World>> world) {
	PROFILE_FUNC();
	PROFILE_FUNC();

	if(!ImGui::Begin("Graphics")) {
		return;
	}
	
	GraphicsSettingsState &state = GraphicsSettings::get_instance().m_state;
	if(ImGui::Checkbox("Ambient Occlusion", &state.m_ambient_occlusion_enabled)) {
		if(world.has_value()) {
			world->get().mark_all_chunks_dirty();
		}
	}
	
	ImGui::End();
}