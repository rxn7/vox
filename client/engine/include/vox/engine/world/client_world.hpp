#pragma once

#include "vox/engine/graphics/renderers/world_renderer.hpp"
#include "vox/common/world/i_world.hpp"
#include "vox/common/networking/packets/s2c_packet.hpp"

class ClientWorld : public IWorld {
public:
	ClientWorld(WorldRenderer &renderer);
	virtual ~ClientWorld();

	void handle_chunk_udate_packet(const S2C_ChunkUpdatePacket &packet);
	void update_dirty_chunk(Chunk *chunk);
	Chunk *try_pop_dirty_chunk();
		
private:
	WorldRenderer &m_renderer;
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};
