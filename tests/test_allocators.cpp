// TODO: Vibe coded, ensure correctness

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "vox/core/allocators/offset_allocator.hpp"

TEST_CASE("OffsetAllocator Core Functionality") {
    // Initialize with 1024 bytes
    const u32 total_size = 1024;
    OffsetAllocator allocator(total_size);
    
    SUBCASE("Initialization State") {
        CHECK(allocator.get_total_memory() == total_size);
        CHECK(allocator.get_used_memory() == 0);
        
        // Should be able to allocate the full size immediately
        auto result = allocator.allocate(total_size);
        REQUIRE(result.has_value());
        CHECK(result->offset == 0);
        CHECK(result->size == total_size);
        CHECK(allocator.get_used_memory() == total_size);
    }

    SUBCASE("Sequential Allocation") {
        auto a1 = allocator.allocate(100);
        auto a2 = allocator.allocate(200);
        auto a3 = allocator.allocate(50);

        REQUIRE(a1.has_value());
        REQUIRE(a2.has_value());
        REQUIRE(a3.has_value());

        // Check offsets are contiguous
        CHECK(a1->offset == 0);
        CHECK(a2->offset == 100);
        CHECK(a3->offset == 300); // 100 + 200

        CHECK(allocator.get_used_memory() == 350);
    }

    SUBCASE("Freeing and Reusing Memory") {
        auto a1 = allocator.allocate(100);
        auto a2 = allocator.allocate(100);
        auto a3 = allocator.allocate(100);

        // Free the middle block (offset 100)
        allocator.free(*a2);
        CHECK(allocator.get_used_memory() == 200);

        // Allocate a small block, should fit in the hole at offset 100
        auto a4 = allocator.allocate(50);
        REQUIRE(a4.has_value());
        CHECK(a4->offset == 100);

        // Allocate another small block, should fit in the remaining hole at 150
        auto a5 = allocator.allocate(50);
        REQUIRE(a5.has_value());
        CHECK(a5->offset == 150);
    }

    SUBCASE("Coalescing: Merge Right") {
        // [Alloc A] [Alloc B]
        auto a = allocator.allocate(100);
        auto b = allocator.allocate(100);
        
        // Free B first. Free space: [100, size 100]
        allocator.free(*b);
        
        // Free A. Should merge with B. Free space: [0, size 200]
        allocator.free(*a);

        // Verify merge by allocating 200 bytes
        auto c = allocator.allocate(200);
        REQUIRE(c.has_value());
        CHECK(c->offset == 0);
        CHECK(c->size == 200);
    }

    SUBCASE("Coalescing: Merge Left") {
        // [Alloc A] [Alloc B]
        auto a = allocator.allocate(100);
        auto b = allocator.allocate(100);

        // Free A first. Free space: [0, size 100]
        allocator.free(*a);

        // Free B. Should merge with A. Free space: [0, size 200]
        allocator.free(*b);

        auto c = allocator.allocate(200);
        REQUIRE(c.has_value());
        CHECK(c->offset == 0);
    }

    SUBCASE("Coalescing: Merge Both Sides (Sandwich)") {
        // [A 100] [B 100] [C 100]
        auto a = allocator.allocate(100);
        auto b = allocator.allocate(100);
        auto c = allocator.allocate(100);

        // Free A and C. Free blocks: [0, 100] and [200, 100]
        allocator.free(*a);
        allocator.free(*c);

        // Free B. Should merge with A (left) and C (right).
        // Result should be one free block [0, 300]
        allocator.free(*b);

        CHECK(allocator.get_used_memory() == 0);

        // Verify we can allocate the full 300 contiguous bytes
        auto d = allocator.allocate(300);
        REQUIRE(d.has_value());
        CHECK(d->offset == 0);
    }

    SUBCASE("Out of Memory (Capacity)") {
        auto a = allocator.allocate(total_size);
        REQUIRE(a.has_value());
        
        // Try to allocate 1 more byte
        auto b = allocator.allocate(1);
        CHECK_FALSE(b.has_value());
    }

    SUBCASE("Fragmentation Failure") {
        // Create fragmentation: [Used 500] [Free 24] [Used 500] ... 
        // Note: Total is 1024.
        
        auto a = allocator.allocate(400); // 0-400
        auto b = allocator.allocate(200); // 400-600
        auto c = allocator.allocate(400); // 600-1000
        // Remaining free: 24 bytes at end.

        // Free the middle chunk
        allocator.free(*b);

        // Current Free Map: [400, 200], [1000, 24]
        // Total free space = 224.
        
        // Try to allocate 210. It fits in TOTAL free space, but not in any contiguous block.
        // The allocator should fail (return nullopt).
        auto d = allocator.allocate(210);
        
        CHECK_FALSE(d.has_value());
        CHECK(allocator.get_used_memory() == 800); // Should not have changed
    }
}