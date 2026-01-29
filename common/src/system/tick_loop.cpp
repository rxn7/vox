#include "vox/common/system/tick_loop.hpp"

void TickLoop::start() {
	m_accumulator = NanosecondsU64(0);
	m_previous_time = HighResClock::now();
}

void TickLoop::accumulate() {
	const HighResTimePoint now = HighResClock::now();
	m_elapsed = now - m_previous_time;
	m_previous_time = now;

	if(m_elapsed > MAX_FRAME_TIME) {
		m_elapsed = MAX_FRAME_TIME;
	}

	m_accumulator += m_elapsed;
}

bool TickLoop::consume_tick() {
	if(m_accumulator >= TICK_DURATION_NS) {
		m_accumulator -= TICK_DURATION_NS;
		return true;
	}

	return false;
}
