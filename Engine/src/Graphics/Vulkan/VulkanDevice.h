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

#include <sngl/Graphics/Device.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace sngl::graphics
{
	class VulkanDevice : public Device
	{
	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;

	public:
		VulkanDevice();
		~VulkanDevice() override;

		void init() override;
	};
}