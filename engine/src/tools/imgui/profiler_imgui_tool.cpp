#include "vox/tools/imgui/profiler_imgui_tool.hpp"
#include "vox/helper/format_helper.hpp"
#include "vox/tools/profiler/profiler.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

void ProfilerImGuiTool::render() {
	PROFILE_FUNC();

	if(!ImGui::Begin("Profiler")) {
		ImGui::End();
		return;
	}

    const u64 mem_usage = get_current_mem_usage();
    ImGui::Text("Mem usage: %.2lu KB", mem_usage);
    
#ifdef NDEBUG
    ImGui::Text("Profiler is disabled in release mode!");
    ImGui::End();
    return;
#else

	ImGui::Checkbox("Paused", &Profiler::get_instance().m_paused);

	if(ImGui::Button("Expand All")) {
		m_expand_all_triggered = true;
	}

	ImGui::SameLine();

	if(ImGui::Button("Hide All")) {
		m_hide_all_triggered = true;
	}
    
    const std::span<const ProfilerNode> data = Profiler::get_instance().get_results();
    
	if(data.empty()) {
		ImGui::Text("No results");
		ImGui::End();
		return;
	}
    
    const ProfilerNode &root = data[0];

	render_node_recursive(data, 0);

	m_expand_all_triggered = false;
	m_hide_all_triggered = false;

	ImGui::End();
#endif
}

void ProfilerImGuiTool::render_node_recursive(std::span<const ProfilerNode> nodes, i16 node_idx) {
#ifndef NDEBUG
	const ProfilerNode &node = nodes[node_idx];

    const f32 duration_ratio_to_total = static_cast<f32>(node.duration_us) / Profiler::get_instance().get_duration_us();

	f32 duration_ratio_to_parent;
	if(node.parent != -1) {
		duration_ratio_to_parent = static_cast<f32>(node.duration_us) / nodes[node.parent].duration_us;
	} else {
		duration_ratio_to_parent = duration_ratio_to_total;
	}
    
    const auto get_entry_color = [](f32 t) -> ImVec4 {
        return ImVec4(
            1.0f,
            glm::min(1.0f, (1.0f - t) * 2.0f),
            glm::max(0.0f, 1.0f - t * 2.0f),
            1.0f
        );
    };

    const ImVec4 fg_color = get_entry_color(duration_ratio_to_parent);

	ImGui::PushID(node_idx);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_Selected;
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

	const i32 duration_to_total_percentage = std::clamp(static_cast<i32>(duration_ratio_to_total * 100), 0, 100);
	const i32 duration_to_parent_percentage = std::clamp(static_cast<i32>(duration_ratio_to_parent * 100), 0, 100);
    const u32 average_duration = static_cast<f32>(node.duration_us) / node.calls;
    
    const std::string avg_str = FormatHelper::duration(std::chrono::microseconds(average_duration));
    const std::string total_str = FormatHelper::duration(std::chrono::microseconds(node.duration_us));

	const bool open = ImGui::TreeNodeEx("##Node", flags, "%s", node.name);
    ImGui::SameLine();
    
    if(node_idx != 0) {
        ImGui::PushStyleColor(ImGuiCol_Text, fg_color);
        ImGui::Text("\t|\tavg %s\t|\t%u cls\t|\ttot %s\t|\t%u%% par\t|\t%u%% tot", avg_str.c_str(), node.calls, total_str.c_str(), duration_to_parent_percentage, duration_to_total_percentage);
        ImGui::PopStyleColor();
    } else {
        ImGui::Text("Root");
    }
    
    if(open) {
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
	ImGui::PopID();
#endif
}

u64 ProfilerImGuiTool::get_current_mem_usage() {
#ifndef __linux__
	return 0;
#endif

    PROFILE_FUNC();
	
	std::ifstream statm("/proc/self/statm");
	if(!statm) {
		std::println("Failed to open /proc/self/statm");
		return 0;
	}

    u64 size, resident, share;
	statm >> size >> resident >> share;
	statm.close();
    
    const u64 page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    const u64 rss = resident * page_size_kb;

    return rss;
}
