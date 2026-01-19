#pragma once

#include "chunk.hpp"

#include <glm/gtx/hash.hpp>
#include <unordered_map>

class World {
public:
	World();
	~World();

	void update();
	
private:
	std::unordered_map<i16vec3, std::unique_ptr<Chunk>> m_chunks;
};
