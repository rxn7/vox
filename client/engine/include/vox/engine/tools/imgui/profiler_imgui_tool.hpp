#pragma once

#include "vox/common/profiler/profiler.hpp"

class ProfilerImGuiTool {
public:
	void render();

private:
	void render_node_recursive(std::span<const ProfilerNode> nodes, i16 node_idx);
	void update();
	u64 get_current_mem_usage();

private:
	bool m_expand_all_triggered = false;
	bool m_hide_all_triggered = false;

	u64 m_mem_used = 0;

private:
	struct Entry {
		const char *m_name;
		f32 m_duration_us;
	};

	struct ProfilerWindowEntryCmp {
		bool operator()(const Entry &a, const Entry &b) const { return a.m_duration_us > b.m_duration_us; }
	};
};
