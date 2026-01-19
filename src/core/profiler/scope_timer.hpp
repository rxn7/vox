#pragma once

#include "profiler.hpp"

class ScopeTimer {
public:
	explicit inline ScopeTimer(const char *name) : m_name(name) { 
        Profiler::get_instance().start_scope(m_name);
        m_start = ProfilerClock::now();
    }

	inline ~ScopeTimer() {
		const auto end = ProfilerClock::now();
		const f32 duration = std::chrono::duration<f32, std::micro>(end - m_start).count();

		Profiler::get_instance().end_scope(duration);
	}

private:
	const char *m_name;
	std::chrono::high_resolution_clock::time_point m_start;
};

#ifndef NDEBUG
template <size_t N>
constexpr auto clean_func_name(const char(&s)[N]) {
    std::string_view v(s, N);

    // strip args: foo(int) -> foo
    v = v.substr(0, v.find('(')); 

    // strip template args: foo<int> -> foo
    // if(v.back() == '>') 
    //     v = v.substr(0, v.rfind('<'));

    // strip return type: void foo -> foo
    const sz space = v.rfind(' ');
    v = (space == v.npos) ? v : v.substr(space + 1);

    std::array<char, N> a{};

    for(sz i = 0; i < v.size(); ++i) 
        a[i] = v[i];

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
#define PROFILE_SCOPE(name) ScopeTimer COMBINE(__scope_timer__, __COUNTER__)(name);
#define PROFILE_FUNC() ScopeTimer COMBINE(__scope_timer__, __COUNTER__)( \
    ({ \
        static constexpr auto _static_name = clean_func_name(RAW_FUNC_SIG); \
        _static_name.data(); \
    }) \
)

#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNC()
#endif
