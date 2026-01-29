#pragma once

#ifndef NDEBUG
#include "vox/common/helper/singleton.hpp"

typedef std::chrono::high_resolution_clock ProfilerClock;
typedef ProfilerClock::time_point ProfilerTimePoint;

struct ProfilerNode {
	const char *name;
	MillisecondsF32 duration_ms = MillisecondsF32(0);
	u16 calls = 0;
	u8 depth = 0;

	i32 parent = -1;
	i32 first_child = -1;
	i32 last_child = -1;
	i32 next_sibling = -1;
};

struct ProfilerThreadContext {
	ProfilerThreadContext(const char *name = nullptr) {
		thread_id = std::this_thread::get_id();

		if(name != nullptr) {
			thread_name = name;
		} else {
			thread_name = "Thread " + std::to_string(std::hash<std::thread::id>{}(thread_id));
		}

		buffer.reserve(4096);
		results.reserve(4096);
	}

	std::string thread_name;
	std::thread::id thread_id;

	std::vector<ProfilerNode> buffer;

	std::vector<ProfilerNode> results;
	std::mutex results_mutex;

	i32 current_node_idx = -1;
};

class Profiler {
SINGLETON_CLASS(Profiler);
public:
	void start_scope(const char *name);
	void end_scope(MillisecondsF32 duration_ms);

	void register_this_thread(const char *name = nullptr);

	// NOTE: its called "consume" because it invalides the results after using it.
	void consume_each_thread_results(std::function<void(std::vector<ProfilerNode> &results, const std::string &thread_name)> callback);

private:
	ProfilerThreadContext &get_local_thread_context();

private:
	std::mutex m_thread_ctx_registry_mutex;
	std::vector<ProfilerThreadContext*> m_thread_ctx_registry;
};
#endif
