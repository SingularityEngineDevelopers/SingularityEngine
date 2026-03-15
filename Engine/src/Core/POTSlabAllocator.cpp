#include "POTSlabAllocator.h"

#include <cstdint>
#include <bit>
#include <algorithm>
#include <stdexcept>

using namespace sngl::core;

void POTSlabAllocator::FixedSizePool::init(size_t blockSize)
{
	assert(blockSize >= sizeof(Node) && "blockSize must be large enough to hold a pointer");
	m_blockSize = blockSize;
}

void POTSlabAllocator::FixedSizePool::supplyNewPage(void* rawMemory, size_t pageSize)
{
	size_t blocksInPage = pageSize / m_blockSize;
	uint8_t* pageStart = static_cast<uint8_t*>(rawMemory);

	for (size_t i = 0; i < blocksInPage; i++)
	{
		uint8_t* currentBlockAddress = pageStart + (m_blockSize * i);
		Node* node = reinterpret_cast<Node*>(currentBlockAddress);
		node->next = m_freeList;
		m_freeList = node;
	}
}

void* POTSlabAllocator::FixedSizePool::pop()
{
	if (!m_freeList)
		return nullptr;

	Node* freeBlock = m_freeList;
	m_freeList = freeBlock->next;

	return freeBlock;
}

void POTSlabAllocator::FixedSizePool::push(void* ptr)
{
	if (!ptr)
		return;

	Node* node = reinterpret_cast<Node*>(ptr);
	node->next = m_freeList;
	m_freeList = node;
}

POTSlabAllocator::POTSlabAllocator(size_t reservationSize)
	: m_commitedSize(0)
{
	assert(sngl::platform::IsInitialized());
	m_pageSize = sngl::platform::GetPageSize();

	m_reservedSize = (reservationSize + m_pageSize - 1) & ~(m_pageSize - 1);
	m_reservedPtr = sngl::platform::memory::Reserve(reservationSize);

	size_t currentPoolSize = MIN_POOL_SIZE;
	for (size_t i = 0; i < POOL_COUNT; i++)
	{
		m_pools[i].init(currentPoolSize);
		currentPoolSize *= 2;
	}
}

POTSlabAllocator::~POTSlabAllocator()
{
	sngl::platform::memory::Release(m_reservedPtr);
}

void* POTSlabAllocator::allocate(size_t size)
{
	const size_t potSize = std::max(MIN_POOL_SIZE, std::bit_ceil<size_t>(size + sizeof(AllocHeader)));
	const int poolIndex = getPoolIndexForSize(potSize);

	void* rawMemory = nullptr;
	if (poolIndex >= 0)
	{
		rawMemory = m_pools[poolIndex].pop();
		if (!rawMemory)
		{
			m_pools[poolIndex].supplyNewPage(requestPage(), m_pageSize);
			rawMemory = m_pools[poolIndex].pop();
		}
	}
	else
	{
		// fallback
		rawMemory = sngl::platform::memory::Reserve(potSize);
		if (!rawMemory)
			throw std::runtime_error("Failed to allocate memory");

		sngl::platform::memory::Commit(rawMemory, potSize);
	}

	assert(rawMemory);
	AllocHeader* allocHeader = reinterpret_cast<AllocHeader*>(rawMemory);
	allocHeader->allocationSize = potSize;

	return reinterpret_cast<uint8_t*>(rawMemory) + sizeof(AllocHeader);
}

int POTSlabAllocator::getPoolIndexForSize(size_t potSize) const
{
	// log2(x) - 4 is array index
	// POT gives us O(1) complexity making this allocator super fast
	// since std::bit_width gives us log2(x) + 1, we need to subtract additional 1
	// https://en.cppreference.com/w/cpp/numeric/bit_width.html
	const int poolIndex = std::bit_width<size_t>(potSize) - 5;
	if (poolIndex > POOL_COUNT - 1)
		return -1;

	return poolIndex;
}

void POTSlabAllocator::free(void* ptr)
{
	const AllocHeader* header = reinterpret_cast<const AllocHeader*>(ptr);
	const int poolIndex = getPoolIndexForSize(header->allocationSize);

	if (poolIndex >= 0)
		m_pools[poolIndex].push(ptr);
	else
		sngl::platform::memory::Release(ptr);
}

void* POTSlabAllocator::requestPage()
{
	uint8_t* currentPtr = static_cast<uint8_t*>(m_reservedPtr) + m_commitedSize;
	uint8_t* newPtr = currentPtr + m_pageSize;
	if (!sngl::platform::memory::Commit(newPtr, m_pageSize))
		throw std::runtime_error("POTSlabAllocator: Failed to commit memory.");

	m_commitedSize += m_pageSize;
	return newPtr;
}