#include "vox/engine/tools/imgui/world_imgui_tool.hpp"
#include "vox/common/helper/format_helper.hpp"
#include "vox/common/world/subchunk.hpp"

void WorldImGuiTool::render(World &world, std::optional<std::reference_wrapper<WorldRenderer>> world_renderer_wrapper) {
    PROFILE_FUNC();

    if(!ImGui::Begin("World")) {
        return;
    }

    ImGui::Text("Sub Chunk size: %s", FormatHelper::memory(sizeof(SubChunk)).c_str());
    ImGui::Text("Chunk size (worst case): %s", FormatHelper::memory(sizeof(SubChunk) * SUBCHUNK_COUNT).c_str());

    if(world_renderer_wrapper.has_value() && ImGui::CollapsingHeader("Renderer")) {
        const WorldRenderer &world_renderer = world_renderer_wrapper->get();

        const OffsetAllocator &vertex_allocator = world_renderer.get_vertex_allocator();
        ImGui::Text("Vertex allocator usage: %s / %s (%d%%)", FormatHelper::memory(vertex_allocator.get_used_memory()).c_str(), FormatHelper::memory(vertex_allocator.get_total_memory()).c_str(), static_cast<u32>(static_cast<f32>(vertex_allocator.get_used_memory()) / vertex_allocator.get_total_memory() * 100));

        const OffsetAllocator &index_allocator = world_renderer.get_index_allocator();
        ImGui::Text("Index allocator usage: %s / %s (%d%%)", FormatHelper::memory(index_allocator.get_used_memory()).c_str(), FormatHelper::memory(index_allocator.get_total_memory()).c_str(), static_cast<u32>(static_cast<f32>(index_allocator.get_used_memory()) / index_allocator.get_total_memory() * 100));

        ImGui::Text("Indirect buffer size: %s", FormatHelper::memory(sizeof(DrawElementsIndirectCommand) * WorldRenderer::MAX_VISIBLE_SUBCHUNKS).c_str());
        ImGui::Text("SubChunk positions ssbo size: %s", FormatHelper::memory(sizeof(u32) * WorldRenderer::MAX_VISIBLE_SUBCHUNKS).c_str());

        ImGui::Text("VBO size: %s", FormatHelper::memory(sizeof(u32) * WorldRenderer::MAX_VERTICES).c_str());
        ImGui::Text("EBO size: %s", FormatHelper::memory(sizeof(u32) * WorldRenderer::MAX_INDICES).c_str());
    }
 
    ImGui::End();
}
