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

BlockID World::get_block(const vec3 &global_position) const {
	const ChunkPosition position = global_position / CHUNK_WIDTH;

	const Chunk *chunk = get_chunk(position);
	if(chunk == nullptr) {
		return BlockID::Air;
	}

	const u8vec3 local_position = u8vec3(
		static_cast<i32>(global_position.x + CHUNK_WIDTH) % CHUNK_WIDTH,
		static_cast<i32>(global_position.y + CHUNK_WIDTH) % CHUNK_WIDTH,
		static_cast<i32>(global_position.z + CHUNK_WIDTH) % CHUNK_WIDTH
	);

	return chunk->get_block(local_position);
}

const Chunk *World::get_chunk(const ChunkPosition &position) const {
	const auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	
	return it->second.get();
}
