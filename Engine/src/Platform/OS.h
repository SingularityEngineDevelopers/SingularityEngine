// Copyright 2026 Tesseract Interactive
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

#ifndef __SNGL_PLATFORM_OS_H_INCLUDED__
#define __SNGL_PLATFORM_OS_H_INCLUDED__

namespace sngl::platform
{
	void Initialize();
	bool IsInitialized();

	size_t GetPageSize();
	size_t GetLargePageMinSize();
	size_t GetAllocationGranularity();

	namespace memory
	{
		void* Reserve(size_t size);
		bool Commit(void* ptr, size_t size);
		void Decommit(void* ptr, size_t size);
		void Release(void* ptr);
	}
}

#endif