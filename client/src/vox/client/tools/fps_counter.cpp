#include "vox/client/tools/fps_counter.hpp"

void FpsCounter::tick() {
	m_frames++;

	const MillisecondsU64 elapsed = std::chrono::duration_cast<MillisecondsU64>(HighResClock::now() - m_last_update);

	if(elapsed.count() >= 1000) {
		m_last_update = HighResClock::now();

		m_frame_rate = m_frames;
		m_frames = 0;
	}
}
