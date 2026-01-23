#pragma once

#include "vox/common/helper/singleton.hpp"

typedef std::chrono::high_resolution_clock ProfilerClock;
typedef ProfilerClock::time_point ProfilerTimePoint;

struct ProfilerNode {
	const char *name;
	u32 duration_us = 0.0f;
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
	void begin(f32 target_duration_us);
	void end();
    void update(f32 dt);

	i16 start_scope(const char *name);
	void end_scope(u32 duration_us);

#ifndef NDEBUG
	inline std::span<const ProfilerNode> get_results() const { 
        return m_results; 
    }

    inline f32 get_duration_us() const {
        return m_duration_us;
    }
    
public:
    bool m_paused = false;

private:
    f32 m_elapsed_us;
    f32 m_duration_us;
    f32 m_target_duration_us;

	std::vector<ProfilerNode> m_buffer;
	std::vector<ProfilerNode> m_results;

	i16 m_current_node_idx = 0;
#endif
};
