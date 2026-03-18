#ifndef __SNGL_GRAPHICS_VULKANRENDERER_H_INCLUDED__
#define __SNGL_GRAPHICS_VULKANRENDERER_H_INCLUDED__

#include <sngl/Graphics/Renderer.h>

#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace sngl::graphics
{
	class VulkanRenderer : public Renderer
	{
	public:
		~VulkanRenderer() override;

		void init() override;

	private:
		VulkanInstance m_instance;
		VulkanDevice m_device;
	};
}

#endif