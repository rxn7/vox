#pragma once

#include <functional>
#include <glm/gtx/hash.hpp>

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
        return std::hash<i16>()(pos.x) ^ std::hash<u8>()(pos.y) ^ std::hash<i16>()(pos.z);
    }
};
