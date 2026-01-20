#pragma once

#include "world/block_id.hpp"
#include "graphics/renderers/chunk_renderer.hpp"

constexpr u32 CHUNK_WIDTH = 16;
constexpr u32 TOTAL_BLOCKS = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;

class World;

class Chunk {
public:
	Chunk(World &world, ivec3 position);

	void render(ChunkRenderer &renderer);

	constexpr vec3 get_global_position() const { 
		return vec3(m_position) * CHUNK_WIDTH; 
	}

	constexpr u16 get_block_idx(u8vec3 v) const {
		return v.y * CHUNK_WIDTH * CHUNK_WIDTH + v.z * CHUNK_WIDTH + v.x;
	}

	constexpr inline BlockID get_block(u8vec3 v) const { 
		return m_blocks[get_block_idx(v)]; 
	}

	inline void set_block(u8 x, u8 y, u8 z, BlockID value) {
		m_blocks[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x] = value;
	}

private:
	void generate_mesh_and_upload(ChunkRenderer &renderer);
	bool should_draw_face(i8 x, i8 y, i8 z) const;

private:
	World &m_world;
	std::array<BlockID, TOTAL_BLOCKS> m_blocks;

	ivec3 m_position;

	bool m_is_dirty = false;
	std::optional<u16> m_render_slot = std::nullopt;
	u32 m_index_count = 0;
};

