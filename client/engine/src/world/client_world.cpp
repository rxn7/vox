#include "vox/engine/world/client_world.hpp"

ClientWorld::ClientWorld(WorldRenderer &renderer) : m_renderer(renderer) {
}

ClientWorld::~ClientWorld() { }

void ClientWorld::handle_chunk_udate_packet(const S2C_ChunkUpdatePacket &packet) {
	Chunk *chunk = get_chunk(packet.position);
	if(chunk == nullptr) {
		chunk = create_chunk(packet.position);
	}

	for(i32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		SubChunk *subchunk = chunk->get_subchunk(i);
		std::shared_ptr<SubChunkData> blocks = packet.data[i];

		if(blocks == nullptr) {
			if(chunk->subchunk_exists(i)) {
				m_renderer.remove_subchunk(*subchunk);
				chunk->remove_subchunk(i);
			}
			continue;
		}

		if(subchunk == nullptr) {
			subchunk = chunk->create_subchunk(i);
		}
		
		subchunk->set_blocks(*blocks);
		chunk->set_dirty(i, true);
	}

	// TODO: Copy blocks
	// chunk->generate();
}
