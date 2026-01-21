#pragma once

#include "vox/graphics/renderers/world_renderer.hpp"
#include "vox/tools/profiler/profiler.hpp"

struct ProfilerWindowEntry {
	const char *name;
	f32 duration_us;
};

struct ProfilerWindowEntryCmp {
	bool operator()(const ProfilerWindowEntry &a, const ProfilerWindowEntry &b) const { return a.duration_us > b.duration_us; }
};

class ProfilerWindow {
public:
	void render(std::optional<std::reference_wrapper<WorldRenderer>> world_renderer_wrapper = std::nullopt);

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
};
