#pragma once

struct ProfilerWindowEntry {
	const char *name;
	f32 duration_us;
};

struct ProfilerWindowEntryCmp {
	bool operator()(const ProfilerWindowEntry &a, const ProfilerWindowEntry &b) const { return a.duration_us > b.duration_us; }
};

class ProfilerWindow {
public:
	void render();

private:
	void render_node_recursive(const std::vector<ProfilerNode> &nodes, i16 node_idx);
	void update_data();

private:
	bool m_paused = false;
	f32 m_time_since_update = 0.0f;
	f32 m_frame_duration_us = 0.0f;
	std::vector<ProfilerNode> m_data;
};
