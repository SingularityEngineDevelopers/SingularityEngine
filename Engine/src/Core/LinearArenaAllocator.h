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
