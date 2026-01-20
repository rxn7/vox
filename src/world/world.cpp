#include "world/world.hpp"

World::World() {
	for(i32 x = -2; x < 2; ++x) {
		for(i32 z = -2; z < 2; ++z) {
			const ChunkPosition position(x, 0, z);
			m_chunks.emplace(position, std::make_unique<Chunk>(*this, position));
		}
	}
}

World::~World() {}

void World::render(ChunkRenderer &renderer) {
	PROFILE_FUNC();

	for(auto &[position, chunk] : m_chunks) {
		chunk->render(renderer);
	}
}

BlockPosition World::global_to_local(vec3 global_position) const {
	PROFILE_FUNC();

	const i32 x = static_cast<i32>(std::floor(global_position.x));
	const i32 y = static_cast<i32>(std::floor(global_position.y));
	const i32 z = static_cast<i32>(std::floor(global_position.z));

	const i32 cx = static_cast<i32>(std::floor(static_cast<f32>(x) / CHUNK_WIDTH));
	const i32 cy = static_cast<i32>(std::floor(static_cast<f32>(y) / CHUNK_WIDTH));
	const i32 cz = static_cast<i32>(std::floor(static_cast<f32>(z) / CHUNK_WIDTH));

	const auto safe_mod = [](i32 a, i32 n) { return (a % n + n) % n; };

	const u8vec3 local_position(
		safe_mod(static_cast<i32>(std::floor(global_position.x)), CHUNK_WIDTH),
		safe_mod(static_cast<i32>(std::floor(global_position.y)), CHUNK_WIDTH),
		safe_mod(static_cast<i32>(std::floor(global_position.z)), CHUNK_WIDTH)
	);

	return BlockPosition{
		.chunk_position = ChunkPosition(cx, cy, cz),
		.local_position = local_position
	};
}

BlockID World::get_block(vec3 global_position) const {
	PROFILE_FUNC();

	const BlockPosition block_position = global_to_local(global_position);

	const Chunk *chunk = get_chunk(block_position.chunk_position);
	if(!chunk)
		return BlockID::Air;

	return chunk->get_block(block_position.local_position);
}

const Chunk *World::get_chunk(ChunkPosition position) const {
	PROFILE_FUNC();

	const auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	
	return it->second.get();
}
