#include "vox/world/world.hpp"
#include "vox/world/block_registry.hpp"
#include "vox/world/chunk.hpp"

World::World() {
	for(i32 x = -4; x < 4; ++x) {
		for(i32 z = -4; z < 4; ++z) {
			const ChunkPosition position(x, 0, z);
			m_chunks.emplace(position, std::make_unique<Chunk>(*this, position));
		}
	}
}

World::~World() {}

void World::render(WorldRenderer &renderer) {
	PROFILE_FUNC();
    
    for(auto &[position, chunk] : m_chunks) {
        chunk->render(renderer);
    }
}

void World::regenerate_all_chunks() {
	PROFILE_FUNC();

	for(auto &[position, chunk] : m_chunks) {
		chunk->set_dirty();
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

			if(block_type.is_solid) {
				return RaycastResult{
					.hit = true,
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
		.hit = false,
		.distance = distance_traveled,
	};
}

BlockID World::get_block(BlockPosition position) const {
	PROFILE_FUNC();

	const Chunk *chunk = get_chunk(position.chunk_position);
	if(chunk == nullptr) {
		return BlockID::Air;
	}

	return chunk->get_block(position.local_position);
}

void World::set_block(BlockPosition position, BlockID value) {
	PROFILE_FUNC();

	Chunk *chunk = get_chunk(position.chunk_position);
	if(chunk == nullptr) {
		return;
	}

	chunk->set_block(position.local_position, value);
	chunk->set_dirty();

	for(i8 i = 0; i < 3; ++i) {
		if(position.local_position[i] == 0 || position.local_position[i] == CHUNK_WIDTH - 1) {
			ivec3 offset(0);
			offset[i] = position.local_position[i] == 0 ? -1 : 1;

			if(Chunk *neighbour_chunk = get_chunk(position.chunk_position + offset)) {
				neighbour_chunk->set_dirty();
			}
		}
	}
}

Chunk *World::get_chunk(ChunkPosition position) const {
	PROFILE_FUNC();

	const auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	
	return it->second.get();
}

BlockPosition::BlockPosition(vec3 global_position) {
	PROFILE_FUNC();

	const i32 x = static_cast<i32>(std::floor(global_position.x));
	const i32 y = static_cast<i32>(std::floor(global_position.y));
	const i32 z = static_cast<i32>(std::floor(global_position.z));

	const auto safe_mod = [](i32 a, i32 n) { return (a % n + n) % n; };

	local_position = {
		safe_mod(x, CHUNK_WIDTH),
		safe_mod(y, CHUNK_WIDTH),
		safe_mod(z, CHUNK_WIDTH)
	};

	chunk_position = {
		static_cast<i32>(std::floor(static_cast<f32>(x) / CHUNK_WIDTH)),
		static_cast<i32>(std::floor(static_cast<f32>(y) / CHUNK_WIDTH)),
		static_cast<i32>(std::floor(static_cast<f32>(z) / CHUNK_WIDTH))
	};
}
