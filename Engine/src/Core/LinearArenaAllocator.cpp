// Copyright 2026 Singularity Engine
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "LinearArenaAllocator.h"
#include <Platform/OS.h>

#include <stdexcept>
#include <cassert>

using namespace sngl::core;

LinearArenaAllocator::LinearArenaAllocator()
	: m_initialized(false), 
	m_baseAddress(nullptr), 
	m_reservedSize(0), 
	m_currentOffset(0), 
	m_commitedSize(0)
{ }

LinearArenaAllocator::~LinearArenaAllocator()
{
	if (m_baseAddress != nullptr)
		sngl::platform::memory::Release(m_baseAddress);
}

void LinearArenaAllocator::init(size_t reservationSize)
{
	assert(!m_initialized);
	m_reservedSize = alignUp(reservationSize, sngl::platform::GetPageSize());
	m_baseAddress = (uint8_t*)sngl::platform::memory::Reserve(m_reservedSize);
	if (!m_baseAddress)
		throw std::runtime_error("Failed to reserve memory");
}

void* LinearArenaAllocator::allocate(size_t size, size_t alignment)
{
	assert(m_initialized);

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
	assert(m_initialized);
	m_currentOffset = 0;
}

size_t LinearArenaAllocator::getCommitedSize() const { return m_commitedSize; }
size_t LinearArenaAllocator::getUsedSize() const { return m_currentOffset; }