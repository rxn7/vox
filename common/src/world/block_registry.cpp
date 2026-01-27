#include "vox/common/world/block_registry.hpp"
#include "vox/common/resources/texture_id.hpp"

// NOTE: its done that way to make explicit assignment to a specific BlockID.
constexpr auto BLOCK_REGISTRY = []() {
	std::array<BlockType, static_cast<sz>(BlockID::SIZE)> blocks{};

#define SET_BLOCK(id, val) blocks[static_cast<sz>(BlockID::id)] = val
	SET_BLOCK(Air, BlockType("Air"));
	SET_BLOCK(Dirt, BlockType("Dirt", TextureID::Dirt));
	SET_BLOCK(Stone, BlockType("Stone", TextureID::Stone));
	SET_BLOCK(Grass, BlockType("Grass", BlockTextures(TextureID::GrassTop, TextureID::GrassSide, TextureID::Dirt)));
	SET_BLOCK(Log, BlockType("Log", BlockTextures(TextureID::LogTopBot, TextureID::LogSide, TextureID::LogTopBot)));
	SET_BLOCK(Leaves, BlockType("Leaves", TextureID::Leaves));

	return blocks;
}();

const BlockType &BlockRegistry::get(BlockID id) {
	return BLOCK_REGISTRY[(size_t)id];
}

