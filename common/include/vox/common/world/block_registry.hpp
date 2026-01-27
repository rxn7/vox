#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/block_type.hpp"

namespace BlockRegistry {
	extern const BlockType &get(BlockID id);

	static constexpr bool is_block_transparent(BlockID id) { 
		if(id == BlockID::Air) {
			return true;
		}

		return get(id).is_transparent();
	}
};
