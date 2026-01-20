#pragma once

#include "world/chunk.hpp"
#include <glm/gtx/hash.hpp>

typedef ivec3 ChunkPosition;

struct BlockPosition {
	ChunkPosition chunk_position;
	u8vec3 local_position;
};

class World {
public:
	World();
	~World();

	void render(ChunkRenderer &renderer);

	BlockPosition global_to_local(vec3 global_position) const;
	BlockID get_block(vec3 global_position) const;
	const Chunk *get_chunk(ChunkPosition position) const;

private:
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
