#ifdef VOX_DEBUG

#include "vox/common/profiler/profiler.hpp"

thread_local ProfilerThreadContext *s_local_ctx = nullptr;

void Profiler::start_scope(const char *name) {
	if(!s_local_ctx) {
		return;
	}

	// try to find the node first
	if(s_local_ctx->current_node_idx != -1) {
		i32 child_idx = s_local_ctx->buffer[s_local_ctx->current_node_idx].first_child;
		while(child_idx != -1) {
			if(s_local_ctx->buffer[child_idx].name == name) { // TODO: we're comparing pointers, not sure if its right
				s_local_ctx->current_node_idx = child_idx;
				return;
			}

			child_idx = s_local_ctx->buffer[child_idx].next_sibling;
		}
	}


	const i32 idx = static_cast<i32>(s_local_ctx->buffer.size());

	ProfilerNode &node = s_local_ctx->buffer.emplace_back();
	node.name = name;
	node.parent = s_local_ctx->current_node_idx;

	if(s_local_ctx->current_node_idx != -1) {
		ProfilerNode &parent = s_local_ctx->buffer[s_local_ctx->current_node_idx];

		node.depth = parent.depth + 1;

		if(parent.first_child == -1) {
			parent.first_child = idx;
		} else {
			s_local_ctx->buffer[parent.last_child].next_sibling = idx;
		}

		parent.last_child = idx;
	}

	s_local_ctx->current_node_idx = idx;
}

void Profiler::end_scope(MillisecondsF32 duration_ms) {
	if(!s_local_ctx) {
		return;
	}

	if(s_local_ctx->current_node_idx == -1) [[unlikely]] {
		std::println("Called Profiler::end_scope on empty stack!");
		return;
	}

	ProfilerNode &node = s_local_ctx->buffer[s_local_ctx->current_node_idx];
	node.duration_ms += duration_ms;
	node.calls++;

	s_local_ctx->current_node_idx = node.parent;

	// parent doesn't exist, so we just popped the root
	if(s_local_ctx->current_node_idx == -1) {
		{
			std::lock_guard<std::mutex> lock(s_local_ctx->results_mutex);
			bool should_swap = false;
			if(s_local_ctx->results.empty()) {
				should_swap = true;
			} else {
				const f32 new_duration = s_local_ctx->buffer[0].duration_ms.count();
				const f32 old_duration = s_local_ctx->results[0].duration_ms.count();

				if(new_duration > old_duration) {
					should_swap = true;
				}
			}

			if(should_swap) {
				std::swap(s_local_ctx->results, s_local_ctx->buffer);
			}
		}

		s_local_ctx->buffer.clear();
	}
}

void Profiler::register_this_thread(const char *name) {
	if(s_local_ctx == nullptr) {
		s_local_ctx = new ProfilerThreadContext(nullptr);

		std::lock_guard<std::mutex> lock(m_thread_ctx_registry_mutex);
		m_thread_ctx_registry.push_back(s_local_ctx);
	}

	if(name != nullptr) {
		s_local_ctx->thread_name = name;
	}
}

void Profiler::consume_each_thread_results(std::function<void(std::vector<ProfilerNode> &results, const std::string &thread_name)> callback) {
	std::vector<ProfilerThreadContext*> contexts_copy;

	{
		std::lock_guard<std::mutex> registry_lock(m_thread_ctx_registry_mutex);
		contexts_copy = m_thread_ctx_registry;
	}

	thread_local std::vector<ProfilerNode> swap_buffer;

	for(ProfilerThreadContext *ctx : contexts_copy) {
		if(!ctx) {
			continue;
		}

		{
			std::lock_guard<std::mutex> results_lock(ctx->results_mutex);
			std::swap(ctx->results, swap_buffer);
		}

		if(!swap_buffer.empty()){ 
			callback(swap_buffer, ctx->thread_name);
			swap_buffer.clear();
		}
	}
}

#endif
