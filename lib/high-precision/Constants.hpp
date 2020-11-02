/*************************************************************************
*  2020 Bronek Kozicki                                                   *
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
namespace math {
	template <typename Rr> const constexpr bool useConstexpr = (std::numeric_limits<Rr>::digits10 <= 33);

	template <int N> struct ConstexprConstantsHP {
		// when they are , certain calculations can be optimized by the compiler.
		static constexpr RealHP<N>    PI          = boost::math::constants::pi<RealHP<N>>();
		static constexpr RealHP<N>    TWO_PI      = boost::math::constants::two_pi<RealHP<N>>();
		static constexpr RealHP<N>    HALF_PI     = boost::math::constants::half_pi<RealHP<N>>();
		static constexpr RealHP<N>    SQRT_TWO_PI = boost::math::constants::root_two_pi<RealHP<N>>();
		static constexpr RealHP<N>    E           = boost::math::constants::e<RealHP<N>>();
		static constexpr ComplexHP<N> I           = ComplexHP<N>(0, 1);

		// these can't be constexpr because there's an fp calculation involved. This could be solved by using the same approach as in boost::math::constants. We will leave this for later.
		static const RealHP<N> DEG_TO_RAD;
		static const RealHP<N> RAD_TO_DEG;

		// See: → https://en.cppreference.com/w/cpp/types/numeric_limits
		//      → https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
		//      → https://en.cppreference.com/w/cpp/types/numeric_limits/max
		static const RealHP<N> EPSILON;  // NOTE: for double it is DBL_EPSILON;
		static const RealHP<N> MAX_REAL; // NOTE: for double it is DBL_MAX;

		// FIXME: remove this one from pkg/dem/ViscoelasticPM.cpp , pkg/dem/Shop_02.cpp , pkg/dem/STLImporter.cpp
		static constexpr RealHP<N> ZERO_TOLERANCE = RealHP<N>(1e-20); // → it is very arbitrary. Don't use it.
	};
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::PI;
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::TWO_PI;
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::HALF_PI;
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::SQRT_TWO_PI;
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::E;
	template <int N> constexpr ComplexHP<N> ConstexprConstantsHP<N>::I;
	template <int N> const RealHP<N>        ConstexprConstantsHP<N>::DEG_TO_RAD = ConstexprConstantsHP<N>::PI / RealHP<N>(180);
	template <int N> const RealHP<N>        ConstexprConstantsHP<N>::RAD_TO_DEG = RealHP<N>(180) / ConstexprConstantsHP<N>::PI;
	template <int N> const RealHP<N>        ConstexprConstantsHP<N>::EPSILON    = std::numeric_limits<RealHP<N>>::epsilon();
	template <int N> const RealHP<N>        ConstexprConstantsHP<N>::MAX_REAL   = std::numeric_limits<RealHP<N>>::max();
	template <int N> constexpr RealHP<N>    ConstexprConstantsHP<N>::ZERO_TOLERANCE;

	template <int N> struct ConstConstantsHP {
		// when they are , certain calculations can be optimized by the compiler.
		static const RealHP<N>    PI;
		static const RealHP<N>    TWO_PI;
		static const RealHP<N>    HALF_PI;
		static const RealHP<N>    SQRT_TWO_PI;
		static const RealHP<N>    E;
		static const ComplexHP<N> I;

		// these can't be constexpr because there's an fp calculation involved. This could be solved by using the same approach as in boost::math::constants. We will leave this for later.
		static const RealHP<N> DEG_TO_RAD;
		static const RealHP<N> RAD_TO_DEG;

		// See: → https://en.cppreference.com/w/cpp/types/numeric_limits
		//      → https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
		//      → https://en.cppreference.com/w/cpp/types/numeric_limits/max
		static const RealHP<N> EPSILON;
		static const RealHP<N> MAX_REAL;

		// FIXME: remove this one from pkg/dem/ViscoelasticPM.cpp , pkg/dem/Shop_02.cpp , pkg/dem/STLImporter.cpp
		static const RealHP<N> ZERO_TOLERANCE; //= RealHP<N>(1e-20); // → it is very arbitrary. Don't use it.
	};
	template <int N> const RealHP<N>    ConstConstantsHP<N>::PI             = boost::math::constants::pi<RealHP<N>>();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::TWO_PI         = boost::math::constants::two_pi<RealHP<N>>();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::HALF_PI        = boost::math::constants::half_pi<RealHP<N>>();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::SQRT_TWO_PI    = boost::math::constants::root_two_pi<RealHP<N>>();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::E              = boost::math::constants::e<RealHP<N>>();
	template <int N> const ComplexHP<N> ConstConstantsHP<N>::I              = ComplexHP<N>(0, 1);
	template <int N> const RealHP<N>    ConstConstantsHP<N>::DEG_TO_RAD     = ConstConstantsHP<N>::PI / RealHP<N>(180);
	template <int N> const RealHP<N>    ConstConstantsHP<N>::RAD_TO_DEG     = RealHP<N>(180) / ConstConstantsHP<N>::PI;
	template <int N> const RealHP<N>    ConstConstantsHP<N>::EPSILON        = std::numeric_limits<RealHP<N>>::epsilon();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::MAX_REAL       = std::numeric_limits<RealHP<N>>::max();
	template <int N> const RealHP<N>    ConstConstantsHP<N>::ZERO_TOLERANCE = RealHP<N>(1e-20);

	template <int N> using ConstantsHP = typename std::conditional<useConstexpr<RealHP<N>>, ConstexprConstantsHP<N>, ConstConstantsHP<N>>::type;


} // namespace math

using Mathr = math::ConstantsHP<1>;
} // namespace yade
