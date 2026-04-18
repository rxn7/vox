#include "vox/client/world/client_world.hpp"

ClientWorld::ClientWorld(WorldRenderer &renderer) : m_renderer(renderer) {
}

ClientWorld::~ClientWorld() { }

void ClientWorld::handle_chunk_update_packet(S2C_ChunkUpdatePacket packet) {
	PROFILE_FUNC();

	std::shared_ptr<Chunk> p_chunk = get_chunk(packet.position);
	if(p_chunk == nullptr) {
		p_chunk = create_chunk(packet.position);
	}

	std::unique_lock<std::shared_mutex> lock(p_chunk->m_mutex);

	i32 vector_idx = 0;

	for(i32 i = 0; i < SUBCHUNK_COUNT; ++i) {
		std::shared_ptr<SubChunk> p_subchunk = p_chunk->get_subchunk(i);

		const bool has_data = (packet.subchunk_mask & (1 << i));

		if(!has_data) {
			if(p_chunk->subchunk_exists(i)) {
				m_renderer.remove_subchunk(*p_subchunk);
				p_chunk->remove_subchunk(i);
			}

			continue;
		}

		if(!p_subchunk) {
			p_subchunk = create_subchunk(p_chunk, i);
		}

		p_subchunk->set_blocks(packet.data[vector_idx++]);
		p_chunk->set_dirty(i, true);
	}
	lock.unlock();

	// flag neighbours dirty
	for(i8 dx = -1; dx <= 1; ++dx) {
		for(i8 dz = -1; dz <= 1; ++dz) {
			const ChunkPosition position = packet.position + ChunkPosition(dx, dz);
			std::shared_ptr<Chunk> neighbour_chunk = get_chunk(position);

			if(neighbour_chunk == nullptr) {
				continue;
			}

			neighbour_chunk->set_all_non_empty_subchunks_dirty();
			m_dirty_chunk_positions.insert(position);
		}
	}
}

void ClientWorld::handle_chunk_unload_packet(S2C_ChunkUnloadPacket packet) {
	PROFILE_FUNC();

	std::shared_ptr<Chunk> chunk = get_chunk(packet.position);
	if(chunk == nullptr) {
		return;
	}

	remove_chunk(packet.position);
}

void ClientWorld::update_dirty_chunk(std::shared_ptr<Chunk> chunk) {
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

std::shared_ptr<Chunk> ClientWorld::try_pop_dirty_chunk() {
	PROFILE_FUNC();

	if(m_dirty_chunk_positions.empty()) {
		return nullptr;
	}

	const auto begin = m_dirty_chunk_positions.begin();

	const ChunkPosition position = *begin;
	m_dirty_chunk_positions.erase(begin);

	return get_chunk(position);
}
