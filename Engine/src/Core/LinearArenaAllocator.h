#ifndef __SNGL_CORE_LINEARARENAALLOCATOR_H_INCLUDED__
#define __SNGL_CORE_LINEARARENAALLOCATOR_H_INCLUDED__

#include <stdint.h>

namespace sngl::core
{
	class LinearArenaAllocator
	{
	private:
		uint8_t* m_baseAddress;
		size_t m_reservedSize;
		size_t m_commitedSize;
		size_t m_currentOffset;

	public:
		LinearArenaAllocator(size_t reserveSize);
		~LinearArenaAllocator();

		template <typename T>
		T* allocate(size_t count)
		{
			return reinterpret_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
		}

		void* allocate(size_t size, size_t alignment = 8);
		void reset();

		size_t getCommitedSize() const;
		size_t getUsedSize() const;

	private:
		inline size_t alignUp(size_t value, size_t alignment) const
		{
			return (value + (alignment - 1)) & ~(alignment - 1);
		}
	};
}

#endif __SNGL_CORE_LINEARARENAALLOCATOR_H_INCLUDED__
