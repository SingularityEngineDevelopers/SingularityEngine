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

#include "VulkanInstance.h"
#include "VulkanGlobals.h"

#include <iostream>
#include <vector>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>

using namespace sngl::graphics;

VulkanInstance::VulkanInstance()
	: m_instance(nullptr), m_debugMessenger(nullptr)
{ }

VulkanInstance::~VulkanInstance()
{ }

void VulkanInstance::init()
{
	m_allocCallbacks = vulkan::getAllocCallbacks();
	volkInitialize();

	std::vector<const char*> instanceExtensions;
	std::vector<const char*> instanceLayers;

	// copy SDL's extensions
	{
		uint32_t count;
		const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
		instanceExtensions.resize(count);
		memcpy(instanceExtensions.data(), sdlExtensions, sizeof(const char*) * count);
	}

	#ifdef SNGL_DEBUG
	instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

	const VkApplicationInfo appInfo =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Singularity Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Singularity Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	const VkInstanceCreateInfo instanceCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		#ifdef SNGL_DEBUG
		.pNext = &m_debugMessengerCreateInfo,
		#else
		.pNext = nullptr,
		#endif
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size()),
		.ppEnabledLayerNames = instanceLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
		.ppEnabledExtensionNames = instanceExtensions.data()
	};

	if (vkCreateInstance(&instanceCreateInfo, m_allocCallbacks, &m_instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance");

	volkLoadInstance(m_instance);

#ifdef SNGL_DEBUG
	if (vkCreateDebugUtilsMessengerEXT(m_instance, &m_debugMessengerCreateInfo, m_allocCallbacks, &m_debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("Failed to create a debug utils messenger");
#endif
}

VkBool32 VulkanInstance::debugMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cout << pCallbackData->pMessage << "\n";
	return VK_FALSE;
}