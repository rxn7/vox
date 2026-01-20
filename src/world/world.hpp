#pragma once

#include "world/chunk.hpp"
#include <glm/gtx/hash.hpp>

typedef ivec3 ChunkPosition;

class World {
public:
	World();
	~World();

	void render(ChunkRenderer &renderer);
	Chunk *get_chunk(const ChunkPosition &position);

private:
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
