#include "vox/core/allocators/offset_allocator.hpp"

OffsetAllocator::OffsetAllocator(u32 total_size) : m_total_size(total_size) {
	m_current_free_space = total_size;
	m_free_blocks[0] = total_size;
}

std::optional<OffsetAllocator::Allocation> OffsetAllocator::allocate(u32 size_needed) {
	if(size_needed > m_current_free_space) {
		std::println("OffsetAllocator: Out of memory: required {} bytes, {} available, {} total", size_needed, m_current_free_space, m_total_size);
		return std::nullopt;
	}

	for(auto it = m_free_blocks.begin(); it != m_free_blocks.end(); ++it) {
		const auto [block_offset, block_size] = *it;
		if(block_size < size_needed) {
			continue;
		}

		m_free_blocks.erase(it);

		// if there's space left, create a new free block of it.
		if(block_size > size_needed) {
			m_free_blocks[block_offset + size_needed] = block_size - size_needed;
		}

		m_current_free_space -= size_needed;
		return Allocation{block_offset, size_needed};
	}

	std::println("OffsetAllocator: Could not find a free block with at least {} bytes", size_needed);
	return std::nullopt;
}

void OffsetAllocator::free(Allocation alloc) {
	m_current_free_space += alloc.size;

	auto it = m_free_blocks.insert({alloc.offset, alloc.size}).first;

	const u32 this_block_end = it->first + it->second;


	auto next_it = std::next(it);
	// if the next block begins where this one ends, merge them
	if(next_it != m_free_blocks.end() && this_block_end == next_it->first) {
		it->second += next_it->second;
		m_free_blocks.erase(next_it); 
	}

	if(it != m_free_blocks.begin()) {
		auto prev_it = std::prev(it);
		const u32 prev_block_end = prev_it->first + prev_it->second;

		// if the previous blocks ends where this one begins, merge them
		if(prev_block_end == it->first) {
			prev_it->second += it->second;
			m_free_blocks.erase(it);
		}
	}
}
