#ifndef __SNGL_CORE_MATH_H_INCLUDED__
#define __SNGL_CORE_MATH_H_INCLUDED__

#include <bit>

namespace sngl::core::math
{
	template <typename T>
	requires std::is_integral_v<T>
	constexpr uint32_t log2(T x)
	{
		uint32_t log = 1;
		while (x >>= 1) log++;
		return log;
	}
}

#endif __SNGL_CORE_MATH_H_INCLUDED__