// Copyright 2026 Tesseract Interactive
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

#ifndef __SNGL_GRAPHICS_DEVICE_H_INCLUDED__
#define __SNGL_GRAPHICS_DEVICE_H_INCLUDED__

#include <memory>
#include "Types.h"

namespace sngl::graphics
{
	class Device
	{
	public:
		using deviceptr_t = std::unique_ptr<Device>;

		virtual ~Device() = default;
		virtual void init() = 0;

		static deviceptr_t Create(RenderApi api);
	};
}

#endif