# Copyright 2026 Singularity Engine
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(SNGL_PLATFORM_SOURCES
	"include/sngl/Platform/SDLWindow.h"
	"src/Platform/SDLWindow.cpp"
	"src/Platform/OS.h"
	"src/Platform/OS_win32.cpp"
)

set(SNGL_CORE_SOURCES 
	"include/sngl/Core/Engine.h"
	"src/Core/Engine.cpp"
	"src/Core/Math.h"
	"src/Core/LinearArenaAllocator.h"
	"src/Core/LinearArenaAllocator.cpp"
	"src/Core/POTSlabAllocator.h"
	"src/Core/POTSlabAllocator.cpp"
)

set(SNGL_GRAPHICS_SOURCES 
	"include/sngl/Graphics/Renderer.h"
	"include/sngl/Graphics/Types.h"
	"src/Graphics/Renderer.cpp"
	"src/Graphics/Vulkan/VulkanInstance.h"
	"src/Graphics/Vulkan/VulkanInstance.cpp"
	"src/Graphics/Vulkan/VulkanDevice.h"
	"src/Graphics/Vulkan/VulkanDevice.cpp"
	"src/Graphics/Vulkan/VulkanGlobals.h"
	"src/Graphics/Vulkan/VulkanGlobals.cpp"
	"src/Graphics/Vulkan/VulkanRenderer.h"
	"src/Graphics/Vulkan/VulkanRenderer.cpp"
)

add_library(SingularityEngine STATIC ${SNGL_PLATFORM_SOURCES} ${SNGL_CORE_SOURCES} ${SNGL_GRAPHICS_SOURCES})
target_link_libraries(
	SingularityEngine
	PRIVATE SDL3::SDL3 volk::volk
)

target_include_directories(
	SingularityEngine
	PUBLIC include
	PRIVATE src
)

# unifying debug mode macro
target_compile_definitions(
	SingularityEngine 
	PRIVATE $<$<CONFIG:Debug>:SNGL_DEBUG>
)

if (SNGL_USE_VULKAN)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		find_package(Vulkan REQUIRED)
	else()
		target_link_libraries(SingularityEngine PRIVATE Vulkan-Headers)
	endif()
endif()