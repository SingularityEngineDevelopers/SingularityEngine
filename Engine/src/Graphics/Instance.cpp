#include <sngl/Graphics/Instance.h>
#include "Vulkan/VulkanInstance.h"

#include <stdexcept>

using namespace sngl::graphics;

std::unique_ptr<Instance> Instance::Create(RenderApi api)
{
	switch (api)
	{
	case RenderApi::Vulkan:
		return std::make_unique<VulkanInstance>();
	case RenderApi::DX12:
		throw std::runtime_error("Not Implemented");
	default:
		return std::make_unique<VulkanInstance>();
	}
}