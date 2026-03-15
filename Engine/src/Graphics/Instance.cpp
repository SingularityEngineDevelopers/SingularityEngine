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