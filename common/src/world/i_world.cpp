#include "vox/common/world/i_world.hpp"

IWorld::~IWorld() { }

Chunk *IWorld::create_chunk(ChunkPosition position) {
	const auto result = m_chunks.insert({position, std::make_unique<Chunk>(*this, position)});

	if(!result.second) {
		return nullptr;
	}

	Chunk *chunk = result.first->second.get();

	m_chunk_added_signal.emit(*chunk);
	return chunk;
}

void IWorld::remove_chunk(ChunkPosition position) {
	PROFILE_FUNC();

	const auto it = m_chunks.find(position);
	if(it == m_chunks.end()) {
		return;
	}
	
	m_chunk_removed_signal.emit(*it->second);
	m_chunks.erase(it);
}

void IWorld::set_block(BlockPosition position, BlockID value) {
	PROFILE_FUNC();

	Chunk *chunk = get_chunk(position.chunk_position);
	if(chunk == nullptr) {
		return;
	}

	chunk->set_block_local(position.local_position, value);

	const u32 sub_idx = position.local_position.y / SUBCHUNK_SIZE;
	const u32 local_y = position.local_position.y % SUBCHUNK_SIZE;

	// m_dirty_chunk_positions.insert(position.chunk_position);
	// chunk->set_dirty(sub_idx, true);

	const u32 lx = position.local_position.x;
	const u32 ly = position.local_position.y;
	const u32 lz = position.local_position.z;

	const i32 x_min = (lx == 0) ? -1 : 0;
	const i32 x_max = (lx == SUBCHUNK_SIZE - 1) ? 1 : 0;

	const i32 z_min = (lz == 0) ? -1 : 0;
	const i32 z_max = (lz == SUBCHUNK_SIZE - 1) ? 1 : 0;

	const u32 current_sub_idx = ly / SUBCHUNK_SIZE;
	const u32 local_y_in_sub = ly % SUBCHUNK_SIZE;

	const i32 y_min = (local_y_in_sub == 0) ? -1 : 0;
	const i32 y_max = (local_y_in_sub == SUBCHUNK_SIZE - 1) ? 1 : 0;

	for(i32 dx = x_min; dx <= x_max; ++dx) {
		for(i32 dz = z_min; dz <= z_max; ++dz) {
			Chunk *target_chunk = chunk;
			if(dx != 0 || dz != 0) {
				target_chunk = get_chunk(ChunkPosition(position.chunk_position.x + dx, position.chunk_position.y + dz));
			}

			if(target_chunk != nullptr) {
				for(i32 dy = y_min; dy <= y_max; ++dy) {
					const i32 target_idx = static_cast<i32>(current_sub_idx) + dy;

					if(target_idx < 0 || target_idx >= SUBCHUNK_SIZE) { 
						continue;
					}

					target_chunk->set_dirty((u32)target_idx, true);
					m_dirty_chunk_positions.insert(target_chunk->get_position());
				}
			}
		}
	}
}

BlockID IWorld::get_block(BlockPosition position) const {
	PROFILE_FUNC();

	if(!position.is_valid()) {
		return BlockID::Air;
	}

	const Chunk *chunk = get_chunk(position.chunk_position);
	if(chunk == nullptr) {
		return BlockID::Air;
	}

	return chunk->get_block_local(position.local_position);
}

Chunk *IWorld::get_chunk(ChunkPosition position) const {
	PROFILE_FUNC();

	const auto it = m_chunks.find(position);
	if(it == m_chunks.end()) {
		return nullptr;
	}
	
	return it->second.get();
}

void IWorld::mark_all_chunks_dirty() {
	PROFILE_FUNC();

	for(auto &[position, chunk] : m_chunks) {
		chunk->set_all_non_empty_subchunks_dirty();
		m_dirty_chunk_positions.insert(position);
	}
}
