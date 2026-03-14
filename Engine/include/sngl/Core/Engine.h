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

#ifndef __SNGL_CORE_ENGINE_H_INCLUDED__
#define __SNGL_CORE_ENGINE_H_INCLUDED__

#include <sngl/Platform/SDLWindow.h>
#include <sngl/Graphics/Instance.h>
#include <memory>

namespace sngl
{
	class Engine
	{
	public:
		using window_t = sngl::platform::SDLWindow;
		using renderer_t = sngl::graphics::Instance;

	private:
		std::unique_ptr<window_t> m_window;
		std::unique_ptr<renderer_t> m_renderer;

		bool m_isRunning = false;

	public:
		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void init();
		void run();
		void shutdown();

	private:
		void processEvents();
		void update(float deltaTime);
		void render();
	};
}

#endif