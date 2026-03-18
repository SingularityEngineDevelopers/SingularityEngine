#include "VulkanGlobals.h"

#include <Core/POTSlabAllocator.h>
#include <Core/LinearArenaAllocator.h>

#include <iostream>

using namespace sngl::graphics;

static constexpr size_t LINEAR_PREALLOC_SIZE = 16 * 1024 * 1024;
static constexpr size_t SLAB_PREALLOC_SIZE = 512 * 1024 * 1024;

using SlabAllocator_t = sngl::core::POTSlabAllocator<SLAB_PREALLOC_SIZE>;
using ArenaAllocator_t = sngl::core::LinearArenaAllocator;

enum class AllocatorType { AT_SLAB, AT_LINEAR };

struct VulkanAllocHeader
{
	static constexpr uint64_t MAGIC_VALUE = std::bit_cast<uint64_t>("VKALLOC");
	uint64_t magic;
	AllocatorType type;
	size_t allocationSize;
};

static void* vulkanAllocate(
	void* pUserData,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope allocationScope
);

static void* vulkanReallocate(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope allocationScope
);

static void vulkanFree(void* pUserData, void* pMemory);

static VkAllocationCallbacks g_allocCbs
{
	.pUserData = nullptr,
	.pfnAllocation = vulkanAllocate,
	.pfnReallocation = vulkanReallocate,
	.pfnFree = vulkanFree,
	.pfnInternalAllocation = nullptr,
	.pfnInternalFree = nullptr
};

static bool g_allocatorsInitialized = false;
static SlabAllocator_t g_slabAllocator;
static ArenaAllocator_t g_linearAllocator;

static inline void checkHeader(const VulkanAllocHeader* pHeader)
{
	if (pHeader->magic != VulkanAllocHeader::MAGIC_VALUE)
	{
		std::cout << "BUG: Vulkan supplied an allocation which hadn't had an allocation header" << "\n";
		std::abort();
	}
}

static void* vulkanAllocate(
	void* pUserData,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope allocationScope)
{
	assert(g_allocatorsInitialized); // calling without initializing an allocator

	const size_t sizeWithHeader = size + sizeof(VulkanAllocHeader);
	void* pMemory = nullptr;

	switch (allocationScope)
	{
	case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
	case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
	case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
	{
		pMemory = g_slabAllocator.allocate(sizeWithHeader);
		VulkanAllocHeader* pHeader = static_cast<VulkanAllocHeader*>(pMemory);
		*pHeader = { VulkanAllocHeader::MAGIC_VALUE, AllocatorType::AT_SLAB, sizeWithHeader };
		break;
	}
		
	case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
	{
		pMemory = g_linearAllocator.allocate(sizeWithHeader);
		VulkanAllocHeader* pHeader = static_cast<VulkanAllocHeader*>(pMemory);
		*pHeader = { VulkanAllocHeader::MAGIC_VALUE, AllocatorType::AT_LINEAR, sizeWithHeader };
		break;
	}
	}

	return static_cast<uint8_t*>(pMemory) + sizeof(VulkanAllocHeader);
}

static void* vulkanReallocate(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope allocationScope)
{
	assert(g_allocatorsInitialized); // calling without initializing an allocator

	void* const pOriginalBegin = static_cast<uint8_t*>(pOriginal) - sizeof(VulkanAllocHeader);
	VulkanAllocHeader* pOriginalHeader = static_cast<VulkanAllocHeader*>(pOriginalBegin);

#ifdef SNGL_DEBUG
	checkHeader(pOriginalHeader);
#endif
	
	const size_t newSizeWithHeader = size + sizeof(VulkanAllocHeader);
	const size_t oldSize = pOriginalHeader->allocationSize;
	pOriginalHeader->allocationSize = newSizeWithHeader;

	void* newMemory = nullptr;

	switch (allocationScope)
	{
	case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
	case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
	case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
		newMemory = g_slabAllocator.allocate(size);
		memcpy(newMemory, pOriginalBegin, oldSize);
		g_slabAllocator.free(pOriginalBegin);
		break;
	case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
		newMemory = g_linearAllocator.allocate(size, alignment);
		memcpy(newMemory, pOriginalBegin, oldSize);
		break;
		// in this case we can't free memory, because it's all flushed after 1 frame
	}

	return newMemory;
}

static void vulkanFree(void* pUserData, void* pMemory)
{
	assert(g_allocatorsInitialized); // calling without initializing an allocator
	const VulkanAllocHeader* const pHeader = static_cast<const VulkanAllocHeader* const>(pMemory);
	
#ifdef SNGL_DEBUG
	checkHeader(pHeader);
#endif

	if (pHeader->type == AllocatorType::AT_SLAB)
		g_slabAllocator.free(pMemory);

	// linear allocator will be reset eventually
	// so there's no need (and no possibility)
	// to handle it
}

void sngl::graphics::vulkan::initAllocators()
{
	assert(!g_allocatorsInitialized); // allocators were intialized already
	g_slabAllocator.init();
	g_linearAllocator.init(LINEAR_PREALLOC_SIZE);
	g_allocatorsInitialized = true;
}

VkAllocationCallbacks* sngl::graphics::vulkan::getAllocCallbacks()
{
	return &g_allocCbs;
}