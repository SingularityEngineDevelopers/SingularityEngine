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

#include <sngl/Core/Engine.h>
#include <sngl/Graphics/Device.h>

using namespace sngl;

Engine::Engine()
{
	m_window = std::make_unique<window_t>();
	m_device = device_t::Create(sngl::graphics::RenderApi::Vulkan);
}

Engine::~Engine()
{
	shutdown();
}

void Engine::init()
{
	m_window->init("Singularity Engine");
	m_device->init();
	m_isRunning = true;
}

void Engine::run()
{
	while (m_isRunning)
	{
		processEvents();

		if (m_isRunning)
		{
			update(0.0f); // TODO: add proper deltaTime later
			render();
		}
	}
}

void Engine::shutdown()
{

}

void Engine::processEvents()
{
	if (!m_window->pollEvents())
		m_isRunning = false;
}

void Engine::update(float deltaTime)
{

}

void Engine::render()
{

}