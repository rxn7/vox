#include "vox/common/world/block_position.hpp"
#include "vox/common/world/world_constants.hpp"

BlockPosition::BlockPosition(vec3 global_position) {
	PROFILE_FUNC();

	const i32 x = static_cast<i32>(std::floor(global_position.x));
	const i32 y = static_cast<i32>(std::floor(global_position.y));
	const i32 z = static_cast<i32>(std::floor(global_position.z));

	const auto safe_mod = [](i32 a, i32 n) { return (a % n + n) % n; };

	local_position = {
		safe_mod(x, CHUNK_WIDTH),
		safe_mod(y, CHUNK_WIDTH),
		safe_mod(z, CHUNK_WIDTH)
	};

	chunk_position = ChunkPosition(
		static_cast<i16>(std::floor(static_cast<f32>(x) / CHUNK_WIDTH)),
		static_cast<i16>(std::floor(static_cast<f32>(y) / CHUNK_WIDTH)),
		static_cast<i16>(std::floor(static_cast<f32>(z) / CHUNK_WIDTH))
    );
}