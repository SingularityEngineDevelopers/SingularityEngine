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

namespace sngl::core
{
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
			void init(size_t blockSize);
			void supplyNewPage(void* rawMemory, size_t pageSize);

			void* pop();
			void push(void* ptr);
		};

		struct AllocHeader
		{
			size_t allocationSize;
		};

	private:
		static constexpr size_t POOL_COUNT = 16;
		static constexpr size_t MIN_POOL_SIZE = 16;

		FixedSizePool m_pools[POOL_COUNT];

		void* m_reservedPtr;
		size_t m_reservedSize;
		size_t m_commitedSize;
		size_t m_pageSize;

	public:
		POTSlabAllocator(size_t reservationSize);
		~POTSlabAllocator();

		void* allocate(size_t size);
		void free(void* ptr);

	private:
		void* requestPage();
		int getPoolIndexForSize(size_t potSize) const;
	};
}

#endif