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

#ifndef __SNGL_CORE_POTSLABALLOCATOR_H_INCLUDED__
#define __SNGL_CORE_POTSLABALLOCATOR_H_INCLUDED__

#include <Platform/OS.h>
#include <cassert>
#include <algorithm>
#include <bit>
#include <stdexcept>

#include <Core/Math.h>

namespace sngl::core
{
	template <size_t reservationSize>
	class POTSlabAllocator
	{
	private:
		class FixedSizePool
		{
		private:
			struct Node { Node* next; };
			Node* m_freeList = nullptr;
			size_t m_blockSize;

		public:
			void init(size_t blockSize)
			{
				assert(blockSize >= sizeof(Node) && "blockSize must be large enough to hold a pointer");
				m_blockSize = blockSize;
			}

			void supplyNewPage(void* rawMemory, size_t pageSize)
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

			void* pop()
			{
				if (!m_freeList)
					return nullptr;

				Node* freeBlock = m_freeList;
				m_freeList = freeBlock->next;

				return freeBlock;
			}

			void push(void* ptr)
			{
				if (!ptr)
					return;

				Node* node = reinterpret_cast<Node*>(ptr);
				node->next = m_freeList;
				m_freeList = node;
			}
		};

		struct AllocHeader
		{
			size_t allocationSize;
		};

	private:
		static constexpr size_t MIN_POOL_SIZE = 16ull;
		static constexpr int POOL_COUNT = math::log2(reservationSize / MIN_POOL_SIZE);

		static_assert(reservationSize >= MIN_POOL_SIZE, "Reservation size must be greater or equal than 16");
		static_assert(MIN_POOL_SIZE * (1 << POOL_COUNT) >= reservationSize);
		
		FixedSizePool m_pools[POOL_COUNT];

		void* m_reservedPtr;
		size_t m_reservedSize;
		size_t m_commitedSize;
		size_t m_pageSize;

	public:
		POTSlabAllocator()
			: m_commitedSize(0)
		{
			assert(sngl::platform::IsInitialized() && "Allocators can only be used after platform initialization");
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

		~POTSlabAllocator()
		{
			sngl::platform::memory::Release(m_reservedPtr);
		}

		void* allocate(size_t size)
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

		void free(void* ptr)
		{
			const AllocHeader* header = reinterpret_cast<const AllocHeader*>(ptr);
			const int poolIndex = getPoolIndexForSize(header->allocationSize);

			if (poolIndex >= 0)
				m_pools[poolIndex].push(ptr);
			else
				sngl::platform::memory::Release(ptr);
		}

	private:
		void* requestPage()
		{
			uint8_t* currentPtr = static_cast<uint8_t*>(m_reservedPtr) + m_commitedSize;
			uint8_t* newPtr = currentPtr + m_pageSize;
			if (!sngl::platform::memory::Commit(newPtr, m_pageSize))
				throw std::runtime_error("POTSlabAllocator: Failed to commit memory.");

			m_commitedSize += m_pageSize;
			return newPtr;
		}

		int getPoolIndexForSize(size_t potSize) const
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
	};
}

#endif