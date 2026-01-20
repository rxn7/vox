#include "world.hpp"

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

Chunk *World::get_chunk(const ChunkPosition &position) {
	const auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	
	return it->second.get();
}
