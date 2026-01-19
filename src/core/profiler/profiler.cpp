#include "profiler.hpp"

SINGLETON_IMPL(Profiler);

#ifdef NDEBUG

void Profiler::new_frame() {
	m_frame_start = ProfilerClock::now();
}
void Profiler::end_frame() {
	m_frame_duration_us = std::chrono::duration<f32, std::micro>(ProfilerClock::now() - m_frame_start).count();
}
i16 Profiler::start_scope(const char *name) { return 0; }
void Profiler::end_scope(f32 duration_us) {}

#else 

void Profiler::new_frame() {
	m_frame_start = ProfilerClock::now();

	m_current_node_idx = 0;
}

void Profiler::end_frame() {
	m_frame_duration_us = std::chrono::duration<f32, std::micro>(ProfilerClock::now() - m_frame_start).count();

	m_results.swap(m_buffer);
	m_buffer.clear();
}

i16 Profiler::start_scope(const char *name) {
	if(m_buffer.empty()) {
        m_buffer.push_back({ .name = "Root", .duration_us = m_frame_duration_us });
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

void Profiler::end_scope(f32 duration_us) {
	ProfilerNode &node = m_buffer[m_current_node_idx];
	node.duration_us += duration_us;
	++node.calls;

	if(node.parent != -1) {
		m_current_node_idx = node.parent;
	}
}

#endif
