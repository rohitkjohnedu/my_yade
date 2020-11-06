/*************************************************************************
*  2020 Bronek Kozicki                                                   *
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <lib/high-precision/Real.hpp>

/*
 * In this file the constants are declared. To help the compiler with optimizations during compilation they are declared as constexpr.
 *
 * The problem however is that they have to be declared four times, because:
 * → When they are constexpr they must be initialized inside a class.
 * → To generate symbols for linker they must be declared second time outside of class, and initialized there if not constexpr.
 * → But they can't be constexpr if Real type has more than 33 decimal places precision.
 *
 * Hence above are three IF conditions. There are two possible solutions:
 * 1. Declare/initialize each variable in four places. This is prone to mistakes, so be careful.
 *    We will use this approach for now, because this code is more often read than written. But be careful when adding more Constants
 *
 * 2. Use BOOST_PP_IF condition for preprocessor. This involves a macro, so be careful, but each variable is declared only once.
 *    You can examine the "nice" macro solution in 'git show 549774eeca2f63a877' or in https://gitlab.com/yade-dev/trunk/-/blob/549774eeca2f63a877/lib/high-precision/Constants.hpp#L77
 *
 * Switching between the two solutions is rather straightforward (if needed). The commit 549774eeca2f63a877 has been heavily tested.
 *    Each variable in there is initialized in single place: in the macro Y_DECLARE_CONSTANTS.
 *    To examine what the compiler actually sees invoke this command:
 *      g++ -E -P lib/high-precision/Constants.hpp  -I ./ -I /usr/include/eigen3 -I /usr/include/python3.7m > /tmp/Out.hpp
 *
 *
 * Here the non-macro approach is used. We can quickly switch to macro solution if needed, use commit 549774eeca2f63a877 for that.
 *
 *
 * About initializng the values like epsilon() or max() see:
 *      → https://en.cppreference.com/w/cpp/types/numeric_limits
 *      → https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 *      → https://en.cppreference.com/w/cpp/types/numeric_limits/max
 */

// TODO: stop using Mathr::ZERO_TOLERANCE in files pkg/dem/ViscoelasticPM.cpp , pkg/dem/Shop_02.cpp , pkg/dem/STLImporter.cpp and remove this arbitrary constant.

namespace forCtags {
struct Constants { // for ctags
	bool PI;   // this is only for ctags, and is not used anywhere.
};
}

namespace yade {
namespace math {
	template <typename Rr> const constexpr bool useConstexprConstants = (std::numeric_limits<Rr>::digits10 <= 33);

// To generate symbols (so that each variable has an address even the constexpr one) we need to declare the symbols outside of the template struct.
// But inside the struct they are static variables.
#define Y_STATIC(USE_TEMPLATE) BOOST_PP_IF(USE_TEMPLATE, template <int N>, static)

// When they are constexpr certain calculations can be optimized by the compiler. But this is not always possible: it depends on the number of decimal places. So use BOOST_PP_IF conditional.
// But some of them, like DEG_TO_RAD can't be constexpr because there's function call or a floating-point calculation involved.
// Not being able to use constexpr for DEG_TO_RAD could be solved by using the same approach as in boost::math::constants. But we will leave this for later.
#define Y_CONST(USE_CONSTEXPR) BOOST_PP_IF(USE_CONSTEXPR, constexpr, const)
#define Y_NAME(USE_NAME, NAME) BOOST_PP_IF(USE_NAME, NAME<N>::, )
#define Y_INIT(USE_TEMPLATE, USE_CONSTEXPR, INIT_VALUE)                                                                                                        \
	BOOST_PP_IF(USE_TEMPLATE, BOOST_PP_IF(USE_CONSTEXPR, , INIT_VALUE), BOOST_PP_IF(USE_CONSTEXPR, INIT_VALUE, ))

	// clang-format off
	// Three arguments:T        : outside of struct put 'template <int N>' otherwise, inside the struct it should have 'static'
	//                   C      : whether to use 'constexpr' keyword.
	//                      NAME: the struct name
#define Y_DECLARE_CONSTANTS(T, C, NAME)                                                                                                                                                \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) PI             Y_INIT( T, C,  = boost::math::constants::pi<RealHP<N>>()                                               ); \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) TWO_PI         Y_INIT( T, C,  = boost::math::constants::two_pi<RealHP<N>>()                                           ); \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) HALF_PI        Y_INIT( T, C,  = boost::math::constants::half_pi<RealHP<N>>()                                          ); \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) SQRT_TWO_PI    Y_INIT( T, C,  = boost::math::constants::root_two_pi<RealHP<N>>()                                      ); \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) E              Y_INIT( T, C,  = boost::math::constants::e<RealHP<N>>()                                                ); \
	Y_STATIC(T) Y_CONST(C) ComplexHP<N> Y_NAME( T, NAME ) I              Y_INIT( T, C,  = ComplexHP<N>(0, 1)                                                                    ); \
	Y_STATIC(T) const      RealHP<N>    Y_NAME( T, NAME ) DEG_TO_RAD     Y_INIT( T, 0,  = Y_NAME(T, NAME) PI / RealHP<N>(180)                                                   ); \
	Y_STATIC(T) const      RealHP<N>    Y_NAME( T, NAME ) RAD_TO_DEG     Y_INIT( T, 0,  = RealHP<N>(180) / Y_NAME(T, NAME) PI                                                   ); \
	Y_STATIC(T) const      RealHP<N>    Y_NAME( T, NAME ) EPSILON        Y_INIT( T, 0,  = std::numeric_limits<RealHP<N>>::epsilon() /* for double it is DBL_EPSILON;*/          ); \
	Y_STATIC(T) const      RealHP<N>    Y_NAME( T, NAME ) MAX_REAL       Y_INIT( T, 0,  = std::numeric_limits<RealHP<N>>::max()     /* for double it is DBL_MAX;    */          ); \
	Y_STATIC(T) Y_CONST(C) RealHP<N>    Y_NAME( T, NAME ) ZERO_TOLERANCE Y_INIT( T, C,  = RealHP<N>(1e-20)                          /* → it is very arbitrary. Don't use it. */ );
	// clang-format on

	template <int N> struct ConstexprConstantsHP {
		Y_DECLARE_CONSTANTS(0, 1, ConstexprConstantsHP)
	};
	Y_DECLARE_CONSTANTS(1, 1, ConstexprConstantsHP)

	template <int N> struct ConstConstantsHP {
		Y_DECLARE_CONSTANTS(0, 0, ConstConstantsHP)
	};
	Y_DECLARE_CONSTANTS(1, 0, ConstConstantsHP)

	template <int N> using ConstantsHP = typename std::conditional<useConstexprConstants<RealHP<N>>, ConstexprConstantsHP<N>, ConstConstantsHP<N>>::type;

} // namespace math

using Mathr = math::ConstantsHP<1>;

#undef Y_STATIC
#undef Y_CONST
#undef Y_NAME
#undef Y_INIT
#undef Y_DECLARE_CONSTANTS

} // namespace yade
