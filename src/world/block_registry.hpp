#pragma once

#include "world/block_id.hpp"
#include "world/block_type.hpp"

namespace BlockRegistry {
    extern const BlockType &get(BlockID id);
};
