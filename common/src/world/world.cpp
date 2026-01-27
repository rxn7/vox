#include "vox/common/world/world.hpp"
#include "vox/common/world/block_registry.hpp"
#include "vox/common/world/chunk.hpp"
#include "vox/common/world/subchunk.hpp"

World::World() { }

World::~World() {}

void World::create_initial_chunks() {
	PROFILE_FUNC();

	for(i32 x = -4; x < 4; ++x) {
		for(i32 z = -4; z < 4; ++z) {
			create_chunk(ChunkPosition(x, z));
		}
	}

	for(auto &[position, chunk] : m_chunks) {
		chunk.generate();
	}
}

void World::create_chunk(ChunkPosition position) {
	const auto result = m_chunks.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(position),
		std::forward_as_tuple(*this, position)
	);

	if(result.second) {
		Chunk &chunk = result.first->second;
		m_chunk_added_signal.emit(chunk);
		chunk.set_all_non_empty_subchunks_dirty();
	}
}

void World::remove_chunk(ChunkPosition position) {
	const auto it = m_chunks.find(position);
	if(it == m_chunks.end()) {
		return;
	}
	
	m_chunk_removed_signal.emit(it->second);
	m_chunks.erase(it);
}

void World::mark_all_chunks_dirty() {
	PROFILE_FUNC();

	for(auto &[position, chunk] : m_chunks) {
		chunk.set_all_non_empty_subchunks_dirty();
	}
}

RaycastResult World::raycast(vec3 start, vec3 dir, f32 max_distance) const {
	PROFILE_FUNC();

	ivec3 grid_position = ivec3(glm::floor(start));
	const ivec3 step_dir = ivec3(glm::sign(dir));

	const vec3 delta_dist = vec3(
		std::abs(dir.x) < 1e-4f ? 1e30f : std::abs(1.0f / dir.x),
		std::abs(dir.y) < 1e-4f ? 1e30f : std::abs(1.0f / dir.y),
		std::abs(dir.z) < 1e-4f ? 1e30f : std::abs(1.0f / dir.z)
	);

	vec3 side_dist = vec3(
		(dir.x < 0) ? (start.x - grid_position.x) * delta_dist.x : (grid_position.x + 1.0f - start.x) * delta_dist.x,
		(dir.y < 0) ? (start.y - grid_position.y) * delta_dist.y : (grid_position.y + 1.0f - start.y) * delta_dist.y,
		(dir.z < 0) ? (start.z - grid_position.z) * delta_dist.z : (grid_position.z + 1.0f - start.z) * delta_dist.z
	);

	ivec3 last_grid_position = grid_position;
	f32 distance_traveled = 0.0f;
	while(distance_traveled < max_distance) {
		const vec3 test_position = vec3(grid_position) + 0.5f;

		const BlockPosition block_position(test_position);
		const BlockID block_id = get_block(block_position);

		if(block_id != BlockID::Air) {
			const BlockType &block_type = BlockRegistry::get(block_id);

			if(block_type.is_solid()) {
				return RaycastResult{
					.did_hit = true,
					.hit_block_position = block_position,
					.previous_grid_position = last_grid_position,
					.distance = distance_traveled
				};
			}
		}

		last_grid_position = grid_position;
		i32 axis = 0;
		if(side_dist[1] < side_dist[0])
			axis = 1;
		if(side_dist[2] < side_dist[axis])
			axis = 2;

		distance_traveled = side_dist[axis];
		side_dist[axis] += delta_dist[axis];
		grid_position[axis] += step_dir[axis];
	}

	return RaycastResult{
		.did_hit = false,
		.distance = distance_traveled,
	};
}

BlockID World::get_block(BlockPosition position) const {
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

void World::set_block(BlockPosition position, BlockID value) {
	PROFILE_FUNC();

	Chunk *chunk = get_chunk(position.chunk_position);
	if(chunk == nullptr) {
		return;
	}

	chunk->set_block_local(position.local_position, value);

	const u32 sub_idx = position.local_position.y / SUBCHUNK_SIZE;
	const u32 local_y = position.local_position.y % SUBCHUNK_SIZE;

	chunk->set_dirty(sub_idx, true);

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
				}
			}
		}
	}
}

Chunk *World::get_chunk(ChunkPosition position) const {
	PROFILE_FUNC();

	const auto it = m_chunks.find(position);
	if(it == m_chunks.end()) {
		return nullptr;
	}
	
	return &it->second;
}
