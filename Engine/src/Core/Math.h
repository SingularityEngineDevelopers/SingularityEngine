#ifndef __SNGL_CORE_MATH_H_INCLUDED__
#define __SNGL_CORE_MATH_H_INCLUDED__

#include <bit>
#include <limits>

namespace sngl::core::math
{
	template <typename T>
	requires std::is_integral_v<T>
	constexpr bool isPOT(T x)
	{
		return (x > 0) && ((x & (x - 1)) == 0);
	}

	template <std::unsigned_integral T>
	constexpr size_t floor_log2(T x)
	{
		if (x == 0) return 0;
		return static_cast<size_t>((std::numeric_limits<T>::digits - 1) - std::countl_zero(x));
	}

	template <std::unsigned_integral T>
	constexpr size_t ceil_log2(T x)
	{
		if (x <= 1) return 0;

		if (std::is_constant_evaluated())
		{
			size_t log = 0;
			T val = x - 1;
			while (val > 0)
			{
				val >>= 1;
				log++;
			}

			return log;
		}
		else
			return std::bit_width(x - 1);
	}
}

static_assert(sngl::core::math::isPOT(2ull));
static_assert(sngl::core::math::floor_log2(2ull) == 1);
static_assert(sngl::core::math::floor_log2(3ull) == 1);
static_assert(sngl::core::math::ceil_log2(2ull) == 1);
static_assert(sngl::core::math::ceil_log2(3ull) == 2);

#endif __SNGL_CORE_MATH_H_INCLUDED__