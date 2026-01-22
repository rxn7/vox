#include "vox/tools/imgui/graphics_imgui_tool.hpp"
#include "vox/graphics/graphics_settings.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

void GraphicsImGuiTool::render(std::optional<std::reference_wrapper<World>> world) {
    PROFILE_FUNC();

    if(!ImGui::Begin("Graphics")) {
        return;
    }
    
    GraphicsSettingsState &state = GraphicsSettings::get_instance().m_state;
    if(ImGui::Checkbox("Ambient Occlusion", &state.ambient_occlusion_enabled)) {
        if(world.has_value()) {
            world->get().regenerate_all_chunks();
        }
    }
    
    ImGui::End();
}