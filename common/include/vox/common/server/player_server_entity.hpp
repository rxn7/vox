#pragma once

#include "vox/common/server/server_entity.hpp"

class PlayerServerEntity : public ServerEntity {
public:
	inline bool has_chunk_loaded(ChunkPosition pos) const {
		return m_loaded_chunks.contains(pos);
	}

	inline void register_chunk_load(ChunkPosition pos) {
		m_loaded_chunks.emplace(pos);
	}

	inline void register_chunk_unload(ChunkPosition pos) {
		m_loaded_chunks.erase(pos);
	}

	inline const std::unordered_set<ChunkPosition> &get_loaded_chunks() const {
		return m_loaded_chunks;
	}

private:
	ChunkPosition m_last_chunks_position;
	std::unordered_set<ChunkPosition> m_loaded_chunks;
};
