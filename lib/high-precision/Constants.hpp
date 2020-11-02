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
	static const Scalar PI;
	static const Scalar HALF_PI;
	static const Scalar TWO_PI;
	static const Scalar MAX_REAL;
	static const Scalar DEG_TO_RAD;
	static const Scalar RAD_TO_DEG;
	static const Scalar EPSILON;
	static const Scalar ZERO_TOLERANCE;
	static Scalar       Sign(Scalar f) { return ::yade::math::sign(f); }              // { if(f<0) return -1; if(f>0) return 1; return 0; }
	static Scalar       UnitRandom() { return ::yade::math::unitRandom(); }           //((double)rand()/((double)(RAND_MAX)))
	static Scalar       SymmetricRandom() { return ::yade::math::symmetricRandom(); } //2.*(((double)rand())/((double)(RAND_MAX)))-1.
};
using Mathr = Math<Real>;

namespace math {


} // namespace math
} // namespace yade

