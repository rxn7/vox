#pragma once

#include "singleton.hpp"
#include "scope_timer.hpp"
#include <chrono>

typedef std::chrono::high_resolution_clock ProfilerClock;
typedef ProfilerClock::time_point ProfilerTimePoint;

struct ProfileResult {
	std::string_view name;
	f32 duration_ms;
};

struct ProfileResultCmp {
	bool operator()(const ProfileResult &a, const ProfileResult &b) const {
		return a.duration_ms > b.duration_ms;
	}
};

class Profiler {
SINGLETON_CLASS(Profiler);
public:
	void start(std::string_view name);
	void stop(std::string_view name);
	void clear();

	[[nodiscard]] inline const std::multiset<ProfileResult, ProfileResultCmp> &get_results() const { return m_results; }

private:
	std::unordered_map<std::string, f32> m_duration_ms_map;
	std::unordered_map<std::string, ProfilerTimePoint> m_start_time_map;
	std::multiset<ProfileResult, ProfileResultCmp> m_results;
};

#ifndef NDEBUG

#define PROFILE_SCOPE(name) ScopeTimer __scope_timer##__LINE__(name);
#define PROFILE_FUNC() ScopeTimer __scope_timer##__LINE__(__PRETTY_FUNCTION__);

#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNC()

#endif
