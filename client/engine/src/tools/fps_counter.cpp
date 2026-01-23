#include "vox/engine/tools/fps_counter.hpp"

void FpsCounter::tick() {
	++m_frames;

	const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_last_update);
	if(elapsed.count() >= 1.0f) {
		m_last_update = std::chrono::high_resolution_clock::now();

		m_frame_rate = m_frames;
		m_frames = 0;
	}
}
