#pragma once

#include "world/chunk.hpp"
#include <glm/gtx/hash.hpp>

typedef ivec3 ChunkPosition;

class World {
public:
	World();
	~World();

	void render(ChunkRenderer &renderer);

	BlockID get_block(const vec3 &global_position) const;
	const Chunk *get_chunk(const ChunkPosition &position) const;

private:
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
