#pragma once

#include "core/block_id.hpp"
#include "core/block_type.hpp"

namespace BlockRegistry {
    extern const BlockType &get(BlockID id);
};
