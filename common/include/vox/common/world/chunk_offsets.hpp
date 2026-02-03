#pragma once

constexpr i32 MAX_SPIRAL_RADIUS = 32;
constexpr u32 SPIRAL_AREA = (2 * MAX_SPIRAL_RADIUS + 1) * (2 * MAX_SPIRAL_RADIUS + 1);

consteval auto generate_spiral_offsets() {
	std::array<i16vec2, SPIRAL_AREA> offsets{};

	u32 idx = 0;
	for(i32 x = -MAX_SPIRAL_RADIUS; x <= MAX_SPIRAL_RADIUS; ++x) {
		for(i32 z = -MAX_SPIRAL_RADIUS; z <= MAX_SPIRAL_RADIUS; ++z) {
			offsets[idx++] = i16vec2(x, z);
		}
	}

	std::sort(offsets.begin(), offsets.end(), [](const i16vec2 a, const i16vec2 b) -> bool {
		return (a.x * a.x + a.y * a.y) < (b.x * b.x + b.y * b.y);
	});

	return offsets;
}

constexpr auto CHUNK_OFFSETS = generate_spiral_offsets();
