#pragma once

#include "core/singleton.hpp"

typedef std::chrono::high_resolution_clock ProfilerClock;
typedef ProfilerClock::time_point ProfilerTimePoint;

struct ProfilerNode {
	const char *name;
	f32 duration_us = 0.0f;
	u16 calls = 0;
	u8 depth = 0;

	i16 parent = -1;
	i16 first_child = -1;
	i16 last_child = -1;
	i16 next_sibling = -1;
};

class Profiler {
SINGLETON_CLASS(Profiler);
public:
	void new_frame();
	void end_frame();

	i16 start_scope(const char *name);
	void end_scope(f32 duration_us);

	inline f32 get_frame_duration_us() const { return m_frame_duration_us; }
#ifndef NDEBUG
	inline std::span<const ProfilerNode> get_results() const { return m_results; }
#endif

private:
    ProfilerTimePoint m_frame_start;
    f32 m_frame_duration_us = 0.0f;

#ifndef NDEBUG
	std::vector<ProfilerNode> m_buffer;
	std::vector<ProfilerNode> m_results;
	i16 m_current_node_idx = 0;
#endif
};
