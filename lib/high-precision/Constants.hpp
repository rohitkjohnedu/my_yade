/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <lib/high-precision/Real.hpp>

namespace forCtags {
struct Constants {
}; // for ctags
}

namespace yade {

template <typename Scalar> struct Math {
	// FIXME: levelOfHP is ignored, thay are all RealHP<1>
	// when they are constexpr, certain calculations can be optimized by the compiler.
	static const constexpr Scalar                             PI          = boost::math::constants::pi<Real>();
	static const constexpr Scalar                             TWO_PI      = boost::math::constants::two_pi<Real>();
	static const constexpr Scalar                             HALF_PI     = boost::math::constants::half_pi<Real>();
	static const constexpr Scalar                             SQRT_TWO_PI = boost::math::constants::root_two_pi<Real>();
	static const constexpr Scalar                             E           = boost::math::constants::e<Real>();
	static const constexpr ComplexHP<math::levelOfHP<Scalar>> I           = Complex(0, 1);

	// these can't be constexpr because there's an fp calculation involved. This could be solved by using the same approach as in boost::math::constants. We will leave this for later.
	static const Scalar DEG_TO_RAD;
	static const Scalar RAD_TO_DEG;

	// TODO: replace with numeric_limits.
	static const constexpr Scalar EPSILON        = DBL_EPSILON;
	static const constexpr Scalar ZERO_TOLERANCE = 1e-20;
	static const constexpr Scalar MAX_REAL       = DBL_MAX;

	// TODO: use directly yade::math::functions
	static Scalar Sign(Scalar f) { return ::yade::math::sign(f); }              // { if(f<0) return -1; if(f>0) return 1; return 0; }
};
using Mathr = Math<Real>;

namespace math {


} // namespace math
} // namespace yade

