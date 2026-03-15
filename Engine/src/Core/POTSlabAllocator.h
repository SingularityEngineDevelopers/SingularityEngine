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