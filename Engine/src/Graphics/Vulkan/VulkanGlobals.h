#ifndef __SNGL_GRAPHICS_VULKANGLOBALS_H_INCLUDED__
#define __SNGL_GRAPHICS_VULKANGLOBALS_H_INCLUDED__

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace sngl::graphics::vulkan
{
	void initAllocators();
	const VkAllocationCallbacks* getAllocCallbacks();
}

#endif