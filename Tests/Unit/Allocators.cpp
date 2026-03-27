#include <gtest/gtest.h>

#include <Core/LinearArenaAllocator.h>
#include <Platform/OS.h>

using LinearArena = sngl::core::LinearArenaAllocator;

static constexpr size_t LINEAR_ARENA_SIZE = 512ull * 1024ull *1024ull;
static LinearArena arenaAllocator;

TEST(LinearArena, allocate)
{
	constexpr size_t ALLOCATION_COUNT = 4ull;
	constexpr size_t ALLOCATION_SIZE = 64ull * 1024ull * 1024ull;
	static_assert(ALLOCATION_COUNT * ALLOCATION_SIZE <= LINEAR_ARENA_SIZE);

	// make a few larger allocations
	for (size_t i = 0; i < ALLOCATION_COUNT; i++)
	{
		void* pMem;
		EXPECT_NO_THROW(pMem = arenaAllocator.allocate(ALLOCATION_SIZE));
		EXPECT_TRUE(pMem != nullptr);
	}
}

TEST(LinearArena, free)
{
	EXPECT_NO_THROW(arenaAllocator.reset());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest();

	sngl::platform::Initialize();
	arenaAllocator.init(LINEAR_ARENA_SIZE);

	return RUN_ALL_TESTS();
}