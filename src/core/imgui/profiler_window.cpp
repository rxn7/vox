#include "profiler_window.hpp"
#include "imgui.h"

void ProfilerWindow::render() {
	PROFILE_FUNC();

	if(!ImGui::Begin("Profiler")) {
		ImGui::End();
		return;
	}

	m_time_since_update += ImGui::GetIO().DeltaTime;
	if(!m_paused && m_time_since_update > 0.25f) {
		m_time_since_update = 0.0f;
		update_data();
	}

	ImGui::Text("Frame Duration: %.2fus", m_frame_duration_us);
	ImGui::Text("FPS: %.0f", 1.0f / (m_frame_duration_us / 1000.0f / 1000.0f));
	ImGui::Checkbox("Paused", &m_paused);

	if(m_data.empty()) {
		ImGui::Text("No results");
		ImGui::End();
		return;
	}

	render_node_recursive(m_data, 0);

	ImGui::End();
}

void ProfilerWindow::render_node_recursive(const std::vector<ProfilerNode> &nodes, i16 node_idx) {
	const ProfilerNode &node = nodes[node_idx];

	float duration_ratio_to_parent;
	if(node.parent != -1) {
		duration_ratio_to_parent = node.duration_us / nodes[node.parent].duration_us;
	} else {
		duration_ratio_to_parent = node.duration_us / m_frame_duration_us;
	}

    const ImVec4 fg_color = ImVec4(
        duration_ratio_to_parent, 
        (1.0f - duration_ratio_to_parent), 
        0.0f, 
        1.0f
    );

	ImGui::PushID(node_idx);
    ImGui::PushStyleColor(ImGuiCol_Text, fg_color);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;
	if(node.first_child == -1) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	} else {
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	}
 
	if(node.depth == 0) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if(ImGui::TreeNodeEx("##Node", flags, "%s %.2fus (%d %%) (%d calls)", node.name, node.duration_us, std::clamp(static_cast<i32>(duration_ratio_to_parent * 100), 0, 100), node.calls)) {
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
}

void ProfilerWindow::update_data() {
	const Profiler &profiler = Profiler::get_instance();

	const std::vector<ProfilerNode> &data = profiler.get_results();
	m_data.clear();
	m_data.assign_range(data);

	m_frame_duration_us = profiler.get_frame_duration_us();
}
