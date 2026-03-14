#include "LinearArenaAllocator.h"
#include <Platform/OS.h>

#include <stdexcept>

using namespace sngl::core;

LinearArenaAllocator::LinearArenaAllocator(size_t reserveSize)
	: m_currentOffset(0), m_commitedSize(0)
{
	m_reservedSize = alignUp(reserveSize, sngl::platform::GetPageSize());

	m_baseAddress = (uint8_t*)sngl::platform::memory::Reserve(m_reservedSize);
	if (!m_baseAddress)
		throw std::runtime_error("Failed to reserve memory");
}

LinearArenaAllocator::~LinearArenaAllocator()
{

}

void* LinearArenaAllocator::allocate(size_t size, size_t alignment)
{
	const size_t alignedOffset = alignUp(m_currentOffset, alignment);
	const size_t newOffset = alignedOffset + size;

	if (alignedOffset + newOffset > m_reservedSize)
		return nullptr;

	if (newOffset > m_commitedSize)
	{
		const size_t pageSize = sngl::platform::GetPageSize();
		const size_t pageAlignedNewCommit = alignUp(newOffset, pageSize);
		const size_t bytesToCommit = newOffset - m_commitedSize;

		if (!sngl::platform::memory::Commit(m_baseAddress + newOffset, bytesToCommit))
			return nullptr;

		m_commitedSize = pageAlignedNewCommit;
	}

	void* result = m_baseAddress + newOffset;
	m_currentOffset = newOffset;
	return result;
}

void LinearArenaAllocator::reset()
{
	m_currentOffset = 0;
}

size_t LinearArenaAllocator::getCommitedSize() const { return m_commitedSize; }
size_t LinearArenaAllocator::getUsedSize() const { return m_currentOffset; }