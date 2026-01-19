#pragma once

constexpr u32 CHUNK_WIDTH = 16;
constexpr u32 TOTAL_BLOCKS = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;

class Chunk {
public:
	Chunk();

	void update();

	constexpr u16 get_block_idx(u8 x, u8 y, u8 z) const {
		return y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x;
	}

	constexpr inline u8 get_block(u8 x, u8 y, u8 z) const { 
		return m_blocks[get_block_idx(x, y, z)]; 
	}

	inline void set_block(u8 x, u8 y, u8 z, u8 value) {
		m_blocks[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x] = value;
	}

private:
	void generate_mesh_and_upload();

private:
	std::array<u8, TOTAL_BLOCKS> m_blocks;

	std::optional<u16> m_render_slot = std::nullopt;
	u32 m_index_count = 0;

	bool m_is_dirty = false;

	friend class World;
};

