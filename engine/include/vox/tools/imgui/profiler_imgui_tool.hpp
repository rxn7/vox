#pragma once

#include "vox/tools/profiler/profiler.hpp"

class ProfilerImGuiTool {
public:
	void render();

private:
	void render_node_recursive(const std::vector<ProfilerNode> &nodes, i16 node_idx);
	void update();
	u64 get_current_mem_usage();

private:
	bool m_paused = false;

	bool m_expand_all_triggered = false;
	bool m_hide_all_triggered = false;

	u64 m_mem_used = 0;

	f32 m_time_since_update = 0.0f;
	f32 m_frame_duration_us = 0.0f;
	std::vector<ProfilerNode> m_data;

private:
    struct Entry {
        const char *name;
        f32 duration_us;
    };

    struct ProfilerWindowEntryCmp {
        bool operator()(const Entry &a, const Entry &b) const { return a.duration_us > b.duration_us; }
    };
};
