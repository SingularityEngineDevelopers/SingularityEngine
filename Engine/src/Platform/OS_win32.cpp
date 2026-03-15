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

#ifdef _WIN32

#include "OS.h"
#include <Windows.h>
#include <cassert>

static bool g_initialized = false;
static size_t g_pageSize = 0;
static size_t g_largePageMinSize = 0;
static size_t g_allocGranularity = 0;

void sngl::platform::Initialize()
{
	assert(!g_initialized && "sngl::platform::Initialize called more than once");

	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	g_pageSize = sysInfo.dwPageSize;
	g_allocGranularity = sysInfo.dwAllocationGranularity;
	g_largePageMinSize = ::GetLargePageMinimum();

	g_initialized = true;
}

bool sngl::platform::IsInitialized()
{
	return g_initialized;
}

size_t sngl::platform::GetPageSize()
{
	assert(g_initialized && "Platform uninitialized");
	return g_pageSize;
}

size_t sngl::platform::GetLargePageMinSize()
{
	assert(g_initialized && "Platform uninitialized");
	return g_largePageMinSize;
}

size_t sngl::platform::GetAllocationGranularity()
{
	assert(g_initialized && "Platform uninitialized");
	return g_allocGranularity;
}

void* sngl::platform::memory::Reserve(size_t size)
{
	assert(g_initialized && "Platform uninitialized");
	return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
}

bool sngl::platform::memory::Commit(void* ptr, size_t size)
{
	assert(g_initialized && "Platform uninitialized");
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != nullptr;
}

void sngl::platform::memory::Decommit(void* ptr, size_t size)
{
	assert(g_initialized && "Platform uninitialized");
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

void sngl::platform::memory::Release(void* ptr)
{
	assert(g_initialized && "Platform uninitialized");
	VirtualFree(ptr, 0, MEM_RELEASE);
}

#endif