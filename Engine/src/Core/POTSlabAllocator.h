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
	class FixedSizeBlock
	{
	private:
		struct Node { Node* next; };
		Node* m_freeList;
		size_t m_blockSize;
		size_t m_pageSize;

	public:
		FixedSizeBlock();

		void init(size_t blockSize);

		void* pop();
		void push(void* ptr);
		void supplyNewMemory(void* ptr, size_t memSize);

		inline size_t getSize() const { return m_blockSize; }
	};

	template <size_t reservationSize>
	class POTSlabAllocator
	{
	private:
		static constexpr size_t getBlockSize(size_t arrIx)
		{
			return (1ull << (arrIx + 4));
		}

		static constexpr int getBlockIxFromSize(size_t blockSize)
		{
			int result = math::ceil_log2(blockSize) - 4;
			if (result > MAX_BLOCK_COUNT)
				return -1;
			
			return result;
		}

		static_assert(math::isPOT(reservationSize), "Reservation size must be a power of 2");
		static constexpr size_t MIN_CELL_SIZE = 16ull;
		static constexpr size_t MAX_BLOCK_COUNT = 16ull;
		static constexpr size_t MAX_EFFECTIVE_ALLOCATION = getBlockSize(MAX_BLOCK_COUNT);

		FixedSizeBlock m_blocks[MAX_BLOCK_COUNT];

		bool m_initialized = false;
		void* m_reservedMemory = nullptr;
		size_t m_currentOffset = 0;
		size_t m_pageSize = 0;

		struct AllocHeader
		{
			static constexpr uint64_t MAGIC_VALUE = std::bit_cast<uint64_t>("POTSLAB");
			uint64_t magicValue;
			size_t allocationSize;
		};

	public:
		void init()
		{
			assert(!m_initialized); // You can't initialize an allocator twice
			m_pageSize = sngl::platform::GetPageSize();
			m_reservedMemory = sngl::platform::memory::Reserve(reservationSize);
			if (!m_reservedMemory)
				throw std::runtime_error("POTSlabAllocator: Failed to reserve memory for allocations");

			for (size_t i = 0; i < MAX_BLOCK_COUNT; i++)
				m_blocks[i].init(getBlockSize(i));

			m_initialized = true;
		}

		void* allocate(size_t size)
		{	
			assert(m_initialized); // Using an uninitialized allocator
			const size_t potSize = std::max(MIN_CELL_SIZE, std::bit_ceil(size + sizeof(AllocHeader)));
			const int blockIx = getBlockIxFromSize(potSize);
			void* memPtr = nullptr;

			if (blockIx >= 0)
			{
				auto& block = m_blocks[blockIx];
				memPtr = block.pop();

				if (!memPtr)
				{
					uint8_t* currentMem = static_cast<uint8_t*>(m_reservedMemory) + m_currentOffset;

					// TODO: handle blocks that are bigger than page size
					// some math may be required for optimization
					if (block.getSize() > m_pageSize)
						throw std::runtime_error("Not Implemented");

					if (!sngl::platform::memory::Commit(currentMem, m_pageSize))
						throw std::runtime_error("POTSlabAllocator: Failed to commit new memory");

					block.supplyNewMemory(currentMem, m_pageSize);
					memPtr = block.pop();
					m_currentOffset += m_pageSize;
				}
			}
			else
			{
				// FALLBACK
				// if we can't allocate in the pool
				// we just allocate memory almost like malloc does
				memPtr = sngl::platform::memory::Reserve(potSize);
				if (!memPtr)
					throw std::runtime_error("POTSlabAllocator: Failed to reserve fallback memory");

				if (!sngl::platform::memory::Commit(memPtr, potSize))
					throw std::runtime_error("POTSlabAllocator: Failed to commit fallback memory");
			}

			AllocHeader* header = static_cast<AllocHeader*>(memPtr);
			header->magicValue = AllocHeader::MAGIC_VALUE;
			header->allocationSize = potSize;

			return reinterpret_cast<uint8_t*>(memPtr) + sizeof(AllocHeader);
		}

		void free(void* ptr)
		{
			AllocHeader* allocHeader = reinterpret_cast<AllocHeader*>(
				static_cast<uint8_t*>(ptr) - sizeof(AllocHeader)
			);

			assert(allocHeader->magicValue == AllocHeader::MAGIC_VALUE); // Wrong memory pointer used
			const int blockIx = getBlockIxFromSize(allocHeader->allocationSize);
			if (blockIx < 0)
				sngl::platform::memory::Release(allocHeader);
			else
				m_blocks->push(allocHeader);
		}
	};
}

#endif