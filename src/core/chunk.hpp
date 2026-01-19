#pragma once

#include "core/block_id.hpp"

constexpr u32 CHUNK_WIDTH = 16;
constexpr u32 TOTAL_BLOCKS = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;

class World;

class Chunk {
public:
	Chunk(World &world, ivec3 position);

	void update();
	void render();

	constexpr u16 get_block_idx(u8 x, u8 y, u8 z) const {
		return y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x;
	}

	constexpr inline BlockID get_block(u8 x, u8 y, u8 z) const { 
		return m_blocks[get_block_idx(x, y, z)]; 
	}

	inline void set_block(u8 x, u8 y, u8 z, BlockID value) {
		m_blocks[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x] = value;
	}

private:
	void generate_mesh_and_upload();
	bool should_draw_face(i8 x, i8 y, i8 z) const;

private:
	World &m_world;
	std::array<BlockID, TOTAL_BLOCKS> m_blocks;

	ivec3 m_position;


	bool m_is_dirty = false;
	std::optional<u16> m_render_slot = std::nullopt;
	u32 m_index_count = 0;
};

