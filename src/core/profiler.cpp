#include "profiler.hpp"

SINGLETON_IMPL(Profiler);

void Profiler::start(std::string_view name) {
	const ProfilerTimePoint now = ProfilerClock::now();
	m_start_time_map.insert({std::string(name), now});
}

void Profiler::stop(std::string_view name) {
	const ProfilerTimePoint now = ProfilerClock::now();
	const ProfilerTimePoint start_time = m_start_time_map.at(std::string(name));

	const f32 duration = std::chrono::duration<f32, std::milli>(now - start_time).count();
	m_duration_ms_map[std::string(name)] += duration;
}

void Profiler::clear() {
	m_results.clear();

	for(const auto &[name, duration_ms] : m_duration_ms_map) {
		m_results.emplace(name, duration_ms);
	}

	m_duration_ms_map.clear();
	m_start_time_map.clear();
}
