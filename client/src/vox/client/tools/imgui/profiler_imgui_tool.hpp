#pragma once

#ifndef NDEBUG
#include "vox/common/profiler/profiler.hpp"

class ProfilerThreadHistory;

class ProfilerImGuiTool {
public:
	void render();

private:
	void render_timeline();
	void render_node_recursive(std::span<const ProfilerNode> nodes, i32 node_idx, f32 total_duration_ms);
	u64 get_current_mem_usage_bytes();

private:
	std::map<std::string, ProfilerThreadHistory> m_histories;
	std::optional<u32> m_selected_frame_age = std::nullopt;

	bool m_paused = false;

	bool m_expand_all_triggered = false;
	bool m_hide_all_triggered = false;

	u64 m_mem_used = 0;
};

class ProfilerThreadHistory {
public:
	void push(std::vector<ProfilerNode> &nodes);
	void clear();
	const std::vector<ProfilerNode> &get_frame(u32 age) const;

	inline const std::vector<std::vector<ProfilerNode>> &get_frames() {
		return m_frames;
	}

	inline const std::vector<f32> &get_plot_data() const {
		return m_plot_values;
	}

	inline u32 get_write_idx() const {
		return m_write_idx;
	}

private:
	std::vector<std::vector<ProfilerNode>> m_frames;
	std::vector<f32> m_plot_values;
	u32 m_write_idx;
};

#endif
