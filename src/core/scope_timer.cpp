#include "scope_timer.hpp"
#include "profiler.hpp"

ScopeTimer::ScopeTimer(std::string_view name)
: m_name(name) {
	Profiler::get_instance().start(name);
}

ScopeTimer::~ScopeTimer() {
	Profiler::get_instance().stop(m_name);
}
