#include "vox/tools/imgui/world_imgui_tool.hpp"
#include "imgui.h"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

void WorldImGuiTool::render(World &world, std::optional<std::reference_wrapper<WorldRenderer>> world_renderer_wrapper) {
    PROFILE_FUNC();

    if(!ImGui::Begin("World")) {
        return;
    }

    if(world_renderer_wrapper.has_value() && ImGui::CollapsingHeader("Renderer")) {
        const WorldRenderer &world_renderer = world_renderer_wrapper->get();

        const OffsetAllocator &vertex_allocator = world_renderer.get_vertex_allocator();
        ImGui::Text("Vertex allocator usage: %dKB / %dKB (%d%%)", vertex_allocator.get_used_memory() / 1024, vertex_allocator.get_total_memory() / 1024, static_cast<u32>(static_cast<f32>(vertex_allocator.get_used_memory()) / vertex_allocator.get_total_memory() * 100));

        const OffsetAllocator &index_allocator = world_renderer.get_index_allocator();
        ImGui::Text("Index allocator usage: %dKB / %dKB (%d%%)", index_allocator.get_used_memory() / 1024, index_allocator.get_total_memory() / 1024, static_cast<u32>(static_cast<f32>(index_allocator.get_used_memory()) / index_allocator.get_total_memory() * 100));
    }
 
    ImGui::End();
}