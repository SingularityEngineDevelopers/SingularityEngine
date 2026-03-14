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

#include <sngl/Platform/SDLWindow.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <cassert>
#include <stdexcept>

#ifdef SNGL_DEBUG
static constexpr float SNGL_WINDOW_SIZE_RATIO = 0.75f;
#else
static constexpr float SNGL_WINDOW_SIZE_RATIO = 1.0f;
#endif

using namespace sngl::platform;

SDLWindow::SDLWindow() : m_pWindow(nullptr), m_width(0), m_height(0), m_resized(false)
{ }

SDLWindow::~SDLWindow()
{
	cleanup();
}

void SDLWindow::init(const std::string& title)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));

	const SDL_DisplayID display = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode* const mode = SDL_GetCurrentDisplayMode(display);

	auto createWindowFlags = SDL_WINDOW_RESIZABLE;
	#ifdef SNGL_USE_VULKAN
	createWindowFlags |= SDL_WINDOW_VULKAN;
	#endif

	m_pWindow = SDL_CreateWindow(
		title.data(),
		(int)(mode->w * SNGL_WINDOW_SIZE_RATIO),
		(int)(mode->h * SNGL_WINDOW_SIZE_RATIO),
		createWindowFlags
	);

	if (!m_pWindow)
		throw std::runtime_error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
}

void SDLWindow::cleanup()
{
	if (m_pWindow)
		SDL_DestroyWindow(m_pWindow);

	SDL_Quit();
}

bool SDLWindow::pollEvents()
{
	assert(m_pWindow);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			return false;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			m_width = event.window.data1;
			m_height = event.window.data2;
			m_resized = true;
			break;
		}
	}

	return true;
}

void SDLWindow::resetResizeFlag()
{
	m_resized = false;
}