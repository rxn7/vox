#pragma once

class TickLoop {
public:

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
	static constexpr NanosecondsU64 TICK_DURATION_NS = NanosecondsU64(1'000'000'000) / TICKS_PER_SECOND;
	static constexpr NanosecondsU64 MAX_FRAME_TIME = NanosecondsU64(250'000'000);
	static constexpr f32 TICK_DURATION_SECONDS = std::chrono::duration_cast<std::chrono::duration<f32>>(TICK_DURATION_NS).count();

private:
	NanosecondsU64 m_accumulator = NanosecondsU64(0);
	NanosecondsU64 m_elapsed = NanosecondsU64(0);
	HighResTimePoint m_previous_time;
};
