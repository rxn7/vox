#pragma once

class TickLoop {
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::high_resolution_clock::time_point;
	using Nanoseconds = std::chrono::duration<u64, std::nano>;

public:
	void start();
	void accumulate();
	bool consume_tick();

	inline f64 get_alpha() const {
		return static_cast<f64>(m_accumulator.count()) / static_cast<f64>(TICK_DURATION_NS.count());
	}

	inline f32 get_elapsed() const {
		return std::chrono::duration_cast<std::chrono::duration<f32>>(m_elapsed).count();
	}

public:
	static constexpr u32 TICKS_PER_SECOND = 20;
	static constexpr Nanoseconds TICK_DURATION_NS = Nanoseconds(1'000'000'000) / TICKS_PER_SECOND;
	static constexpr Nanoseconds MAX_FRAME_TIME = Nanoseconds(250'000'000);
	static constexpr f32 TICK_DURATION_SECONDS = std::chrono::duration_cast<std::chrono::duration<f32>>(TICK_DURATION_NS).count();

private:
	Nanoseconds m_accumulator = Nanoseconds(0);
	Nanoseconds m_elapsed = Nanoseconds(0);
	TimePoint m_previous_time;
};
