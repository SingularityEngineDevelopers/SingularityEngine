# Copyright 2026 Tesseract Interactive
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

cmake_minimum_required(VERSION 4.1)
project(SingularityEngine LANGUAGES CXX)

# CXX Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SNGL_USE_VULKAN "Enable Vulkan backend" ON)
option(SNGL_USE_DX12 "Enable Direct3D 12 backend" OFF)

if (NOT SNGL_USE_VULKAN AND NOT SNGL_USE_DX12)
	message(FATAL_ERROR "You must enable at least one backend to build the engine")
endif()

include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG 683181b47cfabd293e3ea409f838915b8297a4fd
)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
	volk
	GIT_REPOSITORY https://github.com/zeux/volk.git
	GIT_TAG 0b17a763ba5643e32da1b2152f8140461b3b7345
)

FetchContent_MakeAvailable(volk)

add_subdirectory(Engine)
add_subdirectory(Sandbox)