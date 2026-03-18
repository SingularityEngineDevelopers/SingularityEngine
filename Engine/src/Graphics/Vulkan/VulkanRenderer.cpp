#include "VulkanRenderer.h"
#include "VulkanGlobals.h"

using namespace sngl::graphics;

VulkanRenderer::~VulkanRenderer()
{

}

void VulkanRenderer::init()
{
	vulkan::initAllocators();
	m_instance.init();
}