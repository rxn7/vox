#pragma once

#include "core/singleton.hpp"

class FpsCounter {
SINGLETON_CLASS(FpsCounter);
public:
	inline u32 get_frame_rate() const { return m_frame_rate; }
	void tick();

private:
	u32 m_frame_rate = 0;
	u32 m_frames = 0;
	std::chrono::high_resolution_clock::time_point m_last_update;
};
