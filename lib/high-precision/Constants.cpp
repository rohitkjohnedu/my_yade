/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <lib/high-precision/Constants.hpp>

namespace yade {
namespace math {

	// https://stackoverflow.com/questions/14395967/proper-initialization-of-static-constexpr-array-in-class-template
	// should not have a (duplicate) initializer in its namespace scope definition
	// FIXME: levelOfHP is ignored, they are all RealHP<1>
	template <> const constexpr Real    Math<Real>::PI;
	template <> const constexpr Real    Math<Real>::TWO_PI;
	template <> const constexpr Real    Math<Real>::HALF_PI;
	template <> const constexpr Real    Math<Real>::SQRT_TWO_PI;
	template <> const constexpr Real    Math<Real>::E;
	template <> const constexpr Complex Math<Real>::I;

	// these can't be constexpr because there's an fp calculation involved. This could be solved by using the same approach as in boost::math::constants. We will leave this for later.
	template <> const Real Math<Real>::DEG_TO_RAD = Math<Real>::PI / Real(180);
	template <> const Real Math<Real>::RAD_TO_DEG = Real(180) / Math<Real>::PI;

	// TODO: replace with numeric_limits.
	template <> const constexpr Real Math<Real>::EPSILON;
	template <> const constexpr Real Math<Real>::ZERO_TOLERANCE;
	template <> const constexpr Real Math<Real>::MAX_REAL;

} // namespace math
} // namespace yade
