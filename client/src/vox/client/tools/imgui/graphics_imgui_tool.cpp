#include "vox/client/tools/imgui/graphics_imgui_tool.hpp"
#include "vox/client/graphics/graphics_settings.hpp"

void GraphicsImGuiTool::render(std::optional<std::reference_wrapper<ClientWorld>> world) {
	PROFILE_FUNC();
	PROFILE_FUNC();

	if(!ImGui::Begin("Graphics")) {
		ImGui::End();
		return;
	}
	
	GraphicsSettingsState &state = GraphicsSettings::get_instance().m_state;
	if(ImGui::Checkbox("Ambient Occlusion", &state.ambient_occlusion_enabled)) {
		if(world.has_value()) {
			world->get().mark_all_chunks_dirty();
		}
	}
	
	ImGui::End();
}
