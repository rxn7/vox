#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/helper/signal.hpp"

class IWorld {
public:
	virtual ~IWorld();

	virtual std::shared_ptr<Chunk> create_chunk(ChunkPosition position);
	virtual void remove_chunk(ChunkPosition position);
	virtual void set_block(BlockPosition position, BlockID value);
	virtual BlockID get_block(BlockPosition position) const;
	virtual std::shared_ptr<Chunk> get_chunk(ChunkPosition position) const;
	virtual void mark_all_chunks_dirty();

	inline std::shared_ptr<SubChunk> create_subchunk(const std::shared_ptr<Chunk> &p_chunk, u32 idx) {
		 p_chunk->m_subchunks[idx] = std::make_unique<SubChunk>(p_chunk, idx);
		 return p_chunk->m_subchunks[idx];
	}

	inline std::shared_ptr<SubChunk> get_or_create_subchunk(const std::shared_ptr<Chunk> &p_chunk, u32 idx) {
		if(idx >= SUBCHUNK_COUNT) [[unlikely]] {
			std::println("Tried to get subchunk at invalid idx: {}", idx);
			return nullptr;
		}

		if(std::shared_ptr<SubChunk> p_subchunk = p_chunk->get_subchunk(idx)) {
			return p_subchunk;
		}

		return create_subchunk(p_chunk, idx);
	}
	
	std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>> &get_chunks() const { 
		return m_chunks; 
	}
	 
public:
	Signal<Chunk&> m_chunk_added_signal;
	Signal<Chunk&> m_chunk_removed_signal;

protected:
	std::unordered_set<ChunkPosition> m_dirty_chunk_positions;
	mutable std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>> m_chunks; 
};
