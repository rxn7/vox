#include "vox/tools/imgui/profiler_window.hpp"
#include "imgui.h"
#include "vox/core/allocators/offset_allocator.hpp"
#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/tools/profiler/profiler.hpp"
#include "vox/tools/profiler/scope_timer.hpp"

void ProfilerWindow::render(std::optional<std::reference_wrapper<WorldRenderer>> world_renderer_wrapper) {
	PROFILE_FUNC();

	if(!ImGui::Begin("Profiler")) {
		ImGui::End();
		return;
	}

	m_time_since_update += ImGui::GetIO().DeltaTime;
	if(!m_paused && m_time_since_update > 0.25f) {
		m_time_since_update = 0.0f;
		update();
	}

	ImGui::Text("FPS: %.0f", 1.0f / (m_frame_duration_us / 1000.0f / 1000.0f));
	ImGui::Text("Mem usage: %.2f MB", m_mem_used / (1024.0f * 1024.0f));
	ImGui::Checkbox("Paused", &m_paused);

	if(ImGui::Button("Expand All")) {
		m_expand_all_triggered = true;
	}

	ImGui::SameLine();

	if(ImGui::Button("Hide All")) {
		m_hide_all_triggered = true;
	}
    
    if(world_renderer_wrapper.has_value()) {
        if(ImGui::CollapsingHeader("World Renderer")) {
            const WorldRenderer &world_renderer = world_renderer_wrapper->get();

            const OffsetAllocator &vertex_allocator = world_renderer.get_vertex_allocator();
            ImGui::Text("Vertex allocator usage: %d / %d (%d%%)", vertex_allocator.get_used_memory(), vertex_allocator.get_total_memory(), static_cast<u32>(static_cast<f32>(vertex_allocator.get_used_memory()) / vertex_allocator.get_total_memory() * 100));

            const OffsetAllocator &index_allocator = world_renderer.get_index_allocator();
            ImGui::Text("Index allocator usage: %d / %d (%d%%)", index_allocator.get_used_memory(), index_allocator.get_total_memory(), static_cast<u32>(static_cast<f32>(index_allocator.get_used_memory()) / index_allocator.get_total_memory() * 100));
        }
    }

	ImGui::Text("Frame Duration: %.2fus", m_frame_duration_us);

	if(m_data.empty()) {
		ImGui::Text("No results");
		ImGui::End();
		return;
	}

	f32 total_measured_duration_us = 0;
	{
		i32 child = m_data.front().first_child;
		while(child != -1) {
			total_measured_duration_us += m_data[child].duration_us;
			child = m_data[child].next_sibling;
		}
	}

	ImGui::Text("Total measured duration: %.2fus (%.2f%%)", total_measured_duration_us, total_measured_duration_us / m_frame_duration_us * 100.0f);

	render_node_recursive(m_data, 0);

	m_expand_all_triggered = false;
	m_hide_all_triggered = false;

	ImGui::End();
}

void ProfilerWindow::render_node_recursive(const std::vector<ProfilerNode> &nodes, i16 node_idx) {
	const ProfilerNode &node = nodes[node_idx];

	f32 duration_ratio_to_parent;
	if(node.parent != -1) {
		duration_ratio_to_parent = node.duration_us / nodes[node.parent].duration_us;
	} else {
		duration_ratio_to_parent = node.duration_us / m_frame_duration_us;
	}

    const ImVec4 fg_color = ImVec4(
        duration_ratio_to_parent * 2.0f, 
        (1.0f - duration_ratio_to_parent) * 2.0f, 
        0.0f, 
        1.0f
    );

	ImGui::PushID(node_idx);
    ImGui::PushStyleColor(ImGuiCol_Text, fg_color);
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, fg_color);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;
	if(node.first_child == -1) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	} else {
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	}
 
	if(node.depth == 0) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if(m_expand_all_triggered) {
		ImGui::SetNextItemOpen(true);
	} else if(m_hide_all_triggered) {
		ImGui::SetNextItemOpen(false);
	}

	const i32 duration_percentage = std::clamp(static_cast<i32>(duration_ratio_to_parent * 100), 0, 100);

	if(ImGui::TreeNodeEx("##Node", flags, "%s %.2fus (%d %%) (%d calls)", node.name, node.duration_us, duration_percentage, node.calls)) {
		std::vector<i16> children_idxs;
		i16 child = node.first_child;

		while(child != -1) {
			children_idxs.push_back(child);
			child = nodes[child].next_sibling;
		}

		std::sort(children_idxs.begin(), children_idxs.end(), [&](i16 a, i16 b) {
			return nodes[a].duration_us > nodes[b].duration_us;
		});

		for(i16 child_idx : children_idxs) {
			render_node_recursive(nodes, child_idx);
		}

		ImGui::TreePop();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopID();
}

void ProfilerWindow::update() {
	const Profiler &profiler = Profiler::get_instance();

#ifndef NDEBUG
	const std::span<const ProfilerNode> data = profiler.get_results();
	m_data.clear();
	m_data.assign_range(data);
#endif

	m_mem_used = get_current_mem_usage();

	m_frame_duration_us = profiler.get_frame_duration_us();
}

u64 ProfilerWindow::get_current_mem_usage() {
#ifndef __linux__
	return 0;
#endif
	
	std::ifstream statm("/proc/self/statm");
	if(!statm) {
		std::println("Failed to open /proc/self/statm");
		return 0;
	}

	u64 size_pages = 0;
	u64 resident_pages = 0;

	statm >> size_pages >> resident_pages;

	statm.close();

	const u64 page_size = sysconf(_SC_PAGESIZE);
	return page_size * size_pages;
}
