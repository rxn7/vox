#pragma once

#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
public:
	void push(const T &item) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(item);
	}

	bool try_pop(T &out) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if(m_queue.empty()) {
			return false;
		}

		out = std::move(m_queue.front());
		m_queue.pop();

		return true;
	}

private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condition;
};
