#pragma once

// first-fit fixed-size allocator
class OffsetAllocator {
public:
	struct Allocation {
		u32 m_offset = 0;
		u32 m_size = 0;
	};

	OffsetAllocator(u32 total_size);

	std::optional<Allocation> allocate(u32 size_needed);
	void free(Allocation alloc);

	inline u32 get_total_memory() const { 
		return m_total_size; 
	}

	inline u32 get_used_memory() const {
		return m_total_size - m_current_free_space; 
	}

private:
	u32 m_total_size = 0;
	u32 m_current_free_space = 0;

	// <offset, free space>
	std::map<u32, u32> m_free_blocks;
};
