#ifndef NDEBUG

#include "vox/client/tools/imgui/profiler_imgui_tool.hpp"
#include "vox/common/helper/format_helper.hpp"

static constexpr sz MAX_FRAMES = 200;

void ProfilerImGuiTool::render() {
	PROFILE_FUNC();

	if(!ImGui::Begin("Profiler")) {
		ImGui::End();
		return;
	}

	ImGui::Text("Memory usage: %s", FormatHelper::memory(get_current_mem_usage_bytes()).c_str());

	static bool cap_fps = false;
	if(ImGui::Checkbox("Cap FPS", &cap_fps)) {
		glfwSwapInterval(cap_fps ? 2 : 0);
	}

	bool effective_paused = m_paused || m_selected_frame_age.has_value();
	ImGui::Checkbox("Pause timeline", &effective_paused);
	m_paused = effective_paused;

	if(m_paused) {
		if(!m_selected_frame_age.has_value()) {
			m_selected_frame_age = 0; // lock to latest frame
		}
	} else {
		m_selected_frame_age = std::nullopt; // return to live view
	}

	ImGui::SameLine();
	if(ImGui::Button("Clear")) {
		m_histories.clear();
	}

	if(!m_paused) {
		Profiler::get_instance().consume_each_thread_results(
			[this](std::vector<ProfilerNode> &results, const std::string &thread_name) {
				m_histories[thread_name].push(results);
			}
		);
	}

	if(m_histories.empty()) {
		ImGui::Text("NO PROFILER DATA");
	} else {
		render_timeline();
	}

	m_hide_all_triggered = false;
	m_expand_all_triggered = false;

	ImGui::End();
}

void ProfilerImGuiTool::render_timeline() {
	for(auto &[name, history] : m_histories) {
		const std::vector<f32> &plot_data = history.get_plot_data();
		if(plot_data.empty()){ 
			continue;
		}

		const u32 age_to_show = m_selected_frame_age.has_value() 
			? std::min(*m_selected_frame_age, static_cast<u32>(plot_data.size() - 1)) 
			: 0;

		const std::vector<ProfilerNode> &frame = history.get_frame(age_to_show);
		if(frame.empty()) {
			continue;
		}

		const f32 total_duration_ms = frame[0].duration_ms.count();
		const std::string header_text = std::format("{} | {:.2f}ms", name, total_duration_ms);

		ImGui::PushID(name.c_str());
		if(ImGui::CollapsingHeader(header_text.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			const auto [min_it, max_it] = std::minmax_element(plot_data.begin(), plot_data.end());
			const f32 min_duration_ms = *min_it;
			const f32 max_duration_ms = *max_it;

			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x);

			const f32 height = std::min(ImGui::GetWindowContentRegionMax().y, 100.0f);

			ImGui::PushID("Plot");
			ImGui::PlotHistogram("##Timing", plot_data.data(), plot_data.size(), history.get_write_idx(), nullptr, 0.0f, max_duration_ms, ImVec2(0, height));
			ImGui::PopID();

			if(ImGui::IsItemHovered()) {
				ImGui::SetItemTooltip("Click to inspect frame");

				if(ImGui::IsMouseClicked(0)) {
					const f32 t = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / ImGui::GetItemRectSize().x;
					const i32 hovered_idx = glm::clamp(static_cast<i32>(t * plot_data.size()), 0, static_cast<i32>(plot_data.size()) - 1);
					
					const i32 age = plot_data.size() - 1 - hovered_idx;
					m_selected_frame_age = age;
					m_paused = true;
				}
			}

			if (!frame.empty()) {
					render_node_recursive(frame, 0, frame[0].duration_ms.count());
			} else {
				ImGui::TextDisabled("Empty frame data.");
			}
		}
		ImGui::PopID();
	}
}

void ProfilerImGuiTool::render_node_recursive(std::span<const ProfilerNode> nodes, i32 node_idx, f32 total_duration_ms) {
	const ProfilerNode& node = nodes[node_idx];

	const f32 duration_ms = node.duration_ms.count();

	const f32 ratio_total = (total_duration_ms > 0.0f) 
		? duration_ms / total_duration_ms
		: 0.0f;
	
	const f32 ratio_parent = node.parent != -1 && nodes[node.parent].duration_ms.count() != 0
		? duration_ms / nodes[node.parent].duration_ms.count()
		: ratio_total;

	const auto get_color = [](f32 t) -> ImVec4 {
		return ImVec4(1.0f, glm::min(1.0f, (1.0f - t) * 2.0f), glm::max(0.0f, 1.0f - t * 2.0f), 1.0f);
	};

	const ImVec4 text_color = get_color(ratio_parent);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
	
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
	}

	if(m_hide_all_triggered) {
		ImGui::SetNextItemOpen(false);
	}

	ImGui::PushID(node_idx);
	ImGui::PushStyleColor(ImGuiCol_Text, text_color);
	
	bool is_open = ImGui::TreeNodeEx(node.name, flags);
	
	ImGui::PopStyleColor();

	if(ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Duration: %s \nCalls: %u", FormatHelper::duration(node.duration_ms).c_str(), node.calls);
	}

	// --- Columns (Stats) ---
	// Using simple SameLine for layout. (Use ImGui::Table in future for pixel-perfect alignment)
	ImGui::SameLine();
	float avail_w = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail_w - 320.0f); // Right align

	const std::string time_str = FormatHelper::duration(node.duration_ms);
	const std::string avg_str = FormatHelper::duration(
		(node.calls > 0) 
			? MillisecondsF32(duration_ms / node.calls)
			: NanosecondsU32(0)
	);

	ImGui::Text("%s | %u calls | avg %s | %d%%", time_str.c_str(), node.calls, avg_str.c_str(), (int)(ratio_parent * 100));

	if(is_open) {
		if(node.first_child != -1) {
			std::vector<i32> children;
			children.reserve(16);
			
			i32 child = node.first_child;
			while(child != -1) {
				children.push_back(child);
				child = nodes[child].next_sibling;
			}

			std::sort(children.begin(), children.end(), [&](i32 a, i32 b){
				return nodes[a].duration_ms > nodes[b].duration_ms;
			});

			for(i32 child_idx : children) {
				render_node_recursive(nodes, child_idx, total_duration_ms);
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

u64 ProfilerImGuiTool::get_current_mem_usage_bytes() {
#ifndef __linux__
	return 0;
#else
	PROFILE_FUNC();
	PROFILE_FUNC();

	std::ifstream statm("/proc/self/statm");
	if(!statm) {
		std::println("Failed to open /proc/self/statm");
		return 0;
	}

	u64 size, resident, share;
	statm >> size >> resident >> share;
	statm.close();
	
	const u64 page_size = sysconf(_SC_PAGE_SIZE);
	const u64 rss = resident * page_size;

	return rss;
#endif
}

void ProfilerThreadHistory::push(std::vector<ProfilerNode> &nodes) {
	if(nodes.empty()) {
		return;
	}

	if(m_frames.size() < MAX_FRAMES) {
		m_frames.push_back(nodes);
		m_plot_values.push_back(0.0f);
	} else {
		m_frames[m_write_idx] = nodes;
	}

	const f32 duration_ms = nodes[0].duration_ms.count();
	if(m_frames.size() <  MAX_FRAMES) {
		m_plot_values.back() = duration_ms;
	} else {
		m_plot_values[m_write_idx] = duration_ms;
	}

	m_write_idx = (m_write_idx + 1) % MAX_FRAMES;
}

void ProfilerThreadHistory::clear() {
	m_frames.clear();
	m_plot_values.clear();
	m_write_idx = 0;
}

const std::vector<ProfilerNode> &ProfilerThreadHistory::get_frame(u32 age) const {
	if(m_frames.empty()) {
		static std::vector<ProfilerNode> s_empty;
		return s_empty;
	}

	const u32 count = m_frames.size();
	const u32 idx = (m_write_idx + count - 1 - age) % count;

	return m_frames[idx];
}

#endif
