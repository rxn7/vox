#pragma once

#include "vox/common/world/block_id.hpp"
#include "vox/common/world/block_position.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/helper/signal.hpp"

class IWorld {
public:
	virtual ~IWorld();

	Chunk *create_chunk(ChunkPosition position);
	void remove_chunk(ChunkPosition position);
	void set_block(BlockPosition position, BlockID value);
	BlockID get_block(BlockPosition position) const;
	Chunk *get_chunk(ChunkPosition position) const;
	void mark_all_chunks_dirty();
	
	std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> &get_chunks() const { 
		return m_chunks; 
	}
	 
public:
	Signal<Chunk&> m_chunk_added_signal;
	Signal<Chunk&> m_chunk_removed_signal;

protected:
	mutable std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks; 
};
