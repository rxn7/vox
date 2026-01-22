#include "vox/tools/profiler/profiler.hpp"
#include <chrono>

#ifdef NDEBUG
void Profiler::begin() { }

void Profiler::end() { }

i16 Profiler::start_scope([[maybe_unused]] const char *name) { return 0; }

void Profiler::end_scope([[maybe_unused]] u32 duration_us) { }

bool Profiler::should_end([[maybe_unused]] ProfilerClock::duration target_duration) { return false; }
#else 

void Profiler::begin() {
	m_begin_time = ProfilerClock::now();
	m_current_node_idx = 0;
}

void Profiler::end() {
    m_duration = ProfilerClock::now() - m_begin_time;
    m_buffer[0].duration_us = std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count();

	m_results.swap(m_buffer);
	m_buffer.clear();
}

bool Profiler::should_end(ProfilerClock::duration target_duration) {
    const ProfilerTimePoint now = ProfilerClock::now();
    return (now - m_begin_time) >= target_duration;
}

i16 Profiler::start_scope(const char *name) {
	if(m_buffer.empty()) {
        m_buffer.push_back({ .name = "", .duration_us = 1 });
        m_current_node_idx = 0;
    }

	i16 child_idx = m_buffer[m_current_node_idx].first_child;

	while(child_idx != -1) {
		if(m_buffer[child_idx].name == name) {
			m_current_node_idx = child_idx;
			return child_idx;
		}

		child_idx = m_buffer[child_idx].next_sibling;
	}

	assert(m_buffer.size() < std::numeric_limits<i16>::max());

	const u8 new_depth = m_buffer[m_current_node_idx].depth + 1;
	const i16 new_index = static_cast<i16>(m_buffer.size());

	m_buffer.push_back({
		.name = name,
		.depth = new_depth,
		.parent = m_current_node_idx
	});

	ProfilerNode &parent_node = m_buffer[m_current_node_idx];

	if(parent_node.first_child == -1) {
		parent_node.first_child = new_index;
	} else {
		m_buffer[parent_node.last_child].next_sibling = new_index;
	}

	parent_node.last_child = new_index;
	m_current_node_idx = new_index;

	return new_index;
}

void Profiler::end_scope(u32 duration_us) {
	ProfilerNode &node = m_buffer[m_current_node_idx];
	node.duration_us += duration_us;
	++node.calls;

	if(node.parent != -1) {
		m_current_node_idx = node.parent;
	}
}

#endif
