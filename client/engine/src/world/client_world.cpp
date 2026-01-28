#include "vox/engine/world/client_world.hpp"

ClientWorld::ClientWorld(WorldRenderer &renderer) : m_renderer(renderer) {
}

ClientWorld::~ClientWorld() { }

void ClientWorld::handle_chunk_udate_packet(const S2C_ChunkUpdatePacket &packet) {
	PROFILE_FUNC();

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

	// flag neighbours dirty
	for(i8 dx = -1; dx <= 1; ++dx) {
		for(i8 dz = -1; dz <= 1; ++dz) {
			const ChunkPosition position = packet.position + ChunkPosition(dx, dz);
			Chunk *chunk = get_chunk(position);

			if(chunk == nullptr) {
				continue;
			}

			chunk->set_all_non_empty_subchunks_dirty();
			m_dirty_chunk_positions.insert(position);
		}
	}
}

void ClientWorld::update_dirty_chunk(Chunk *chunk) {
	PROFILE_FUNC();

	for(const auto &subchunk : chunk->get_subchunks()) {
		if(subchunk == nullptr) {
			continue;
		}

		if(!chunk->is_dirty(subchunk->get_idx())) {
			continue;
		}

		if(subchunk->is_empty()) {
			m_renderer.remove_subchunk(*subchunk);
			chunk->remove_subchunk(subchunk->get_idx());
			continue;
		}

		m_renderer.update_subchunk(*subchunk);
	}
}

Chunk *ClientWorld::try_pop_dirty_chunk() {
	PROFILE_FUNC();

	if(m_dirty_chunk_positions.empty()) {
		return nullptr;
	}

	const auto begin = m_dirty_chunk_positions.begin();

	const ChunkPosition position = *begin;
	m_dirty_chunk_positions.erase(begin);

	return get_chunk(position);
}
