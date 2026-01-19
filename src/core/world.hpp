#pragma once

#include "chunk.hpp"

#include <glm/gtx/hash.hpp>
#include <unordered_map>

typedef ivec3 ChunkPosition;

class World {
public:
	World();
	~World();

	void update();
	Chunk *get_chunk(const ChunkPosition &position);

private:
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
