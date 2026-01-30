#include "vox/common/server/server_world.hpp"
#include "vox/common/world/chunk.hpp"

ServerWorld::ServerWorld() { }

ServerWorld::~ServerWorld() {}

void ServerWorld::create_initial_chunks() {
	PROFILE_FUNC();

	for(i32 x = -4; x < 4; ++x) {
		for(i32 z = -4; z < 4; ++z) {
			Chunk *chunk = create_chunk(ChunkPosition(x, z));
			generate_chunk(*chunk);
		}
	}
}

void ServerWorld::generate_chunk(Chunk &chunk) {
	PROFILE_FUNC();

	// TODO: Temporary
	for(u32 x = 0; x < CHUNK_WIDTH; ++x) {
		for(u32 z = 0; z < CHUNK_WIDTH; ++z) {
			const u32 max_y = SUBCHUNK_SIZE * 4 - 1;

			for(u32 y = 0; y < max_y; ++y) {
				if(y == max_y - 1) {
					chunk.set_block_local(LocalBlockPosition(x, y, z), BlockID::Grass);

					// Trees (temporary)
					if(rand() % 20 == 0) {
						bool has_neighbor = false;
						for(i8 dx = -1; dx <= 1; ++dx) {
							for(i8 dz = -1; dz <= 1; ++dz) {
								if(dx == 0 && dz == 0) {
									continue;
								}

								if(chunk.get_block_relative(x + dx, y+1, z + dz) != BlockID::Air) {
									has_neighbor = true;
									break;
								}
							}
						}

						if(has_neighbor) {
							continue;
						}

						const u8 tree_height = 4 + rand() % 2;

						i8 radius = 2;
						for(u32 dy = tree_height-1; dy <= tree_height + 1; ++dy) {
							for(i8 dx = -radius; dx <= radius; ++dx) {
								for(i8 dz = -radius; dz <= radius; ++dz) {
									chunk.set_block_relative(x + dx, y + dy, z + dz, BlockID::Leaves);
								}
							}

							radius = glm::max(0, radius - 1);
						}

						for(u32 i = 0; i < tree_height - 1; ++i) {
							chunk.set_block_local(LocalBlockPosition(x, y + i, z), BlockID::Log);
						}
					}
				} else if(y < max_y - 5) {
					chunk.set_block_local(LocalBlockPosition(x, y, z), BlockID::Stone);
				} else {
					chunk.set_block_local(LocalBlockPosition(x, y, z), BlockID::Dirt);
				}
			}
		}
	}
}
