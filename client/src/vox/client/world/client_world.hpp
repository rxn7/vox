#pragma once

#include "vox/client/graphics/renderers/world_renderer.hpp"
#include "vox/common/world/i_world.hpp"
#include "vox/common/networking/packets/s2c_packet.hpp"

class ClientWorld : public IWorld {
public:
	ClientWorld(WorldRenderer &renderer);
	virtual ~ClientWorld();

	void handle_chunk_update_packet(S2C_ChunkUpdatePacket packet);
	void handle_chunk_unload_packet(S2C_ChunkUnloadPacket packet);

	void update_dirty_chunk(std::shared_ptr<Chunk> chunk);

	std::shared_ptr<Chunk> try_pop_dirty_chunk();
		
private:
	WorldRenderer &m_renderer;
};
