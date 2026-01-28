#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/helper/signal.hpp"

class IWorld {
public:
	virtual ~IWorld();

	virtual Chunk *create_chunk(ChunkPosition position);
	virtual void remove_chunk(ChunkPosition position);
	virtual void set_block(BlockPosition position, BlockID value); // returns chunk that is containing this block
	virtual BlockID get_block(BlockPosition position) const;
	virtual Chunk *get_chunk(ChunkPosition position) const;
	virtual void mark_all_chunks_dirty();
	
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> &get_chunks() const { 
		return m_chunks; 
	}
	 
public:
	Signal<Chunk&> m_chunk_added_signal;
	Signal<Chunk&> m_chunk_removed_signal;

protected:
	std::unordered_set<ChunkPosition> m_dirty_chunk_positions;
	mutable std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
