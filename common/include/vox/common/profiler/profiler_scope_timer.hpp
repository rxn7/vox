#pragma once

#ifndef NDEBUG
#include "vox/common/profiler/profiler.hpp"

class ProfilerScopeTimer {
public:
	explicit inline ProfilerScopeTimer(const char *name) { 
		Profiler::get_instance().start_scope(name);
		m_start = ProfilerClock::now();
	}

	inline ~ProfilerScopeTimer() {
		const ProfilerTimePoint end = ProfilerClock::now();
		const u32 duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();

		Profiler::get_instance().end_scope(duration);
	}

private:
	ProfilerTimePoint m_start;
};
#endif

#ifndef NDEBUG
template <size_t N>
constexpr auto clean_func_name(const char(&s)[N]) {
	std::string_view v(s, N - 1);

	// strip args: foo(i32) -> foo
	if(const auto pos = v.find('('); pos != std::string_view::npos) {
		v = v.substr(0, v.find('(')); 
	}

	// strip return type: void foo -> foo
	const sz space = v.rfind(' ');
	if(space != std::string_view::npos) {
		v = v.substr(space + 1);
	}

	std::array<char, N> a{};

	for(sz i = 0; i < v.size(); ++i) 
		a[i] = v[i];

	a[v.size()] = '\0';

	return a;
}

#if defined(__clang__) || defined(__GNUC__)
	#define RAW_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	#define RAW_FUNC_SIG __FUNCSIG__
#else
	#define RAW_FUNC_SIG __func__
#endif

#define GLUE(a, b) a##b
#define COMBINE(a, b) GLUE(a, b)

#define PROFILE_SCOPE(name) ProfilerScopeTimer COMBINE(__scope_timer__, __COUNTER__)(name);
#define PROFILE_FUNC() \
	static constexpr auto COMBINE(__func_name_, __LINE__) = clean_func_name(RAW_FUNC_SIG); \
	ProfilerScopeTimer COMBINE(__scope_timer__, __COUNTER__)(COMBINE(__func_name_, __LINE__).data())

#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNC()
#endif
