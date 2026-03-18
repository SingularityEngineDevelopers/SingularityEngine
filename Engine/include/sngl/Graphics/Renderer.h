#ifndef __SNGL_GRAPHICS_RENDERER_H_INCLUDED__
#define __SNGL_GRAPHICS_RENDERER_H_INCLUDED__

#include "Types.h"

#include <memory>

namespace sngl::graphics
{
	class Renderer
	{
	public:
		using RendererPtr_t = std::unique_ptr<Renderer>;

		virtual ~Renderer() = default;

		virtual void init() = 0;

		static RendererPtr_t Create(RenderApi api);
	};
}

#endif