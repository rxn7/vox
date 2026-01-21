#pragma once

#include "vox/world/block_id.hpp"
#include "vox/world/block_type.hpp"

namespace BlockRegistry {
    extern const BlockType &get(BlockID id);
};
