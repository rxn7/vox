#pragma once

#include "vox/common/world/chunk.hpp"
#include "vox/common/world/i_world.hpp"

class ServerWorld : public IWorld {
public:
	ServerWorld();
	virtual ~ServerWorld();
	
	void create_initial_chunks();
	void generate_chunk(Chunk &chunk);
};
