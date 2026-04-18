#pragma once

#include <functional>

typedef i16vec2 ChunkPosition;

struct SubChunkPosition {
	inline bool operator ==(const SubChunkPosition &other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	i16 x;
	u8 y;
	i16 z;
};

template<>
struct std::hash<SubChunkPosition> {
	sz operator()(const SubChunkPosition &pos) const {
		uint64_t x = static_cast<uint16_t>(pos.x);
		uint64_t y = static_cast<uint8_t>(pos.y);
		uint64_t z = static_cast<uint16_t>(pos.z);

		uint64_t packed = (x << 24) | (z << 8) | y;

		return std::hash<uint64_t>()(packed);
	}
};
