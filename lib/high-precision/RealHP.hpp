/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADE_REAL_HP_TYPE_HPP
#define YADE_REAL_HP_TYPE_HPP

// In this file two things are declared for the users:
//
// (1) a RealHP<N> type is declared in such a way that:
//    RealHP<1> == Real
//    RealHP<2> == has twice the precision of Real
//    RealHP<3> == has three times the precision of Real (this one is mainly for compatibility with the counting, usually this type is "slow")
//    RealHP<4> == has four  times the precision of Real (when https://github.com/boostorg/multiprecision/issues/184 is done it will be fast)
//    ...
//    RealHP<N> == has N     times the precision of Real (all the higher types are based on eiher MPFR or boost::cpp_bin_float)
//
// (2) int levelOfRealHP<A> equal to the value 'N' in RealHP<N>, provided that 'A' is a valid RealHP<N> type, otherwise it causes a compilation error.

// note: N in the RealHP<N> -- the type of N has to be signed int, so that compiler can catch errors when -1 is passed as argument.
// Older compiler could convert -1 to large positive number which results in runtime segfault.

#ifndef YADE_REAL_MATH_NAMESPACE
// it must be included before MathFunctions.hpp, otherwise he templates there won't work properly (some types of RealHP<…> will be unknown).
#error "This file cannot be included alone, include Real.hpp instead"
#endif

#include <boost/mpl/at.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/is_complex.hpp>
#include <boost/utility/enable_if.hpp>
#ifdef YADE_MPFR
#include <boost/multiprecision/mpfr.hpp>
#else
#include <boost/multiprecision/cpp_bin_float.hpp>
#endif
#if YADE_REAL_BIT <= 32
#include "ThinComplexWrapper.hpp"
#include "ThinRealWrapper.hpp"
// after the above two:
#include "NumericLimits.hpp"
#endif
#if YADE_REAL_BIT <= 64
#include <boost/multiprecision/float128.hpp>
#endif

namespace yade {
namespace math {
	namespace detail {
		/*************************************************************************/
		/************************* MPFR / cpp_bin_float **************************/
		/*************************************************************************/
		// Here is declared the 'ultimate' precision type called NthLevelRealHP. Depending on settings it can be MPFR or cpp_bin_float.
#ifdef YADE_MPFR
		// if MPFR is available, then use it for higher types. YADE_MPFR is defined when yade links with MPFR after compilation. It is unrelated about how the Real type is defined.
		template <int DecPlaces> using RealHPBackend = boost::multiprecision::mpfr_float_backend<DecPlaces, boost::multiprecision::allocate_stack>;
#else
		// otherwise use boost::cpp_bin_float
		template <int DecPlaces> using RealHPBackend = boost::multiprecision::cpp_bin_float<DecPlaces>;
#endif

		// the NthLevelRealHP uses MPFR or boost::cpp_bin_float to declare type with requested precision
		template <int Level>
		using NthLevelRealHP = boost::multiprecision::number<RealHPBackend<std::numeric_limits<Real>::digits10 * Level>, boost::multiprecision::et_off>;

		/*************************************************************************/
		/*************************    TypeListRealHP    **************************/
		/*************************************************************************/
		// Here is defined the progressing ladder of types 'TypeListRealHP', each next tyle has higher precision, than the previous type.
		// depending on precision specified in compilation options, the types to use are a bit different. They "shift upwards".

#if YADE_REAL_BIT <= 32
		// magic constant 'long double' could be named WrappedReal or WrappedRealHP<1> or UnderlyingReal<1> or RealHP<1>, think about this.
		// note: IsWrapped and UnderlyingRealHP depend fact that ⇘ uses long double. It's a workaround boost python losing 3 digits of precision and a test of correctness.
		using TypeListRealHP = boost::mpl::vector<Real, double, ThinRealWrapper<long double>, NthLevelRealHP<4>, boost::multiprecision::float128>;
#elif YADE_REAL_BIT <= 64
		// later quad-double will be added to this list, https://github.com/boostorg/multiprecision/issues/184
		using TypeListRealHP = boost::mpl::vector<Real, boost::multiprecision::float128>;
#elif YADE_REAL_BIT <= 80
		// Here it might become interesting in few years when templatized versions of doubling-* and quadding-* of types are implemented in boost::multiprecision.
		// Then it will be high speed, high precision. Because long double although not standarized is pretty fast, and then if it's doubled or quadded it might be good.
		using TypeListRealHP = boost::mpl::vector<Real>;
#elif YADE_REAL_BIT <= 128
		// later quad-double will be added here
		using TypeListRealHP = boost::mpl::vector<Real>;
#elif defined(YADE_REAL_MPFR)
		using TypeListRealHP = boost::mpl::vector<Real>;
#elif defined(YADE_REAL_BBFLOAT)
		using TypeListRealHP = boost::mpl::vector<Real>;
#endif
		// some helper template values
		// PosRealHP is searching for A in the TypeListRealHP. If it's found then it returns levelOfRealHP==position_in_TypeListRealHP+1, otherwise it returns 0.
		const constexpr auto                       HPSizeTL      = boost::mpl::size<TypeListRealHP>::value;
		template <typename A> const constexpr auto FindPosRealHP = boost::mpl::find<TypeListRealHP, A>::type::pos::value;
		template <typename A> const constexpr auto PosRealHP     = (FindPosRealHP<A> == HPSizeTL) ? (0) : (FindPosRealHP<A> + 1);
		// calculates the N-th Level in the TypeListRealHP or MPFR or cpp_bin_float
		template <typename A>
		const constexpr auto NthLevel = std::numeric_limits<typename std::decay<A>::type>::digits10 / std::numeric_limits<Real>::digits10;
		// checks if A is an MPFR or cpp_bin_float
		template <typename A> const constexpr bool IsNthLevel = std::is_same<NthLevelRealHP<NthLevel<A>>, typename std::decay<A>::type>::value;
		// The level of RealHP. Returns 0 if A is not a RealHP<…> type (is not a type from TypeListRealHP and is not an NthLevelRealHP), but A is some other unrecognized type.
		template <typename A> const constexpr auto CheckHPLevel = (IsNthLevel<A>) ? (NthLevel<A>) : (PosRealHP<A>);
	} // namespace detail

// FIXME - maybe move more stuff to ::detail
// This helper is needed to workaround broken tgamma for boost::float128
#ifdef BOOST_MP_FLOAT128_HPP
	template <typename A> const constexpr bool IsFloat128 = std::is_same<boost::multiprecision::float128, typename std::decay<A>::type>::value;
#else
	template <typename A> const constexpr bool IsFloat128 = false;
#endif
	// This helper type is used to identify ThinRealWrapper.
#if (defined(YADE_THIN_REAL_WRAPPER_HPP) and defined(YADE_THIN_COMPLEX_WRAPPER_HPP))
	template <typename A> const constexpr bool IsWrapped = std::is_same<ThinRealWrapper<long double>, typename std::decay<A>::type>::value;
	template <typename A> using MakeWrappedComplex       = ThinComplexWrapper<A>;
#else
	template <typename A> const constexpr bool IsWrapped = false;
	template <typename A> using MakeWrappedComplex = A;
#endif
	// This helper type extracts UnderlyingReal (strips ThinRealWrapper) from type 'A' which may be any of the RealHP<N> types.
	// This implementation might be revised in the future if ThinRealWrapper or UnderlyingReal will acquire some new meaning. But interface shall remain the same.
	template <typename A> using UnderlyingRealHP             = typename std::conditional<IsWrapped<A>, long double, A>::type;
	template <typename A> using UnderlyingComplexHP_fromReal = std::complex<UnderlyingRealHP<A>>;

	/*************************************************************************/
	/*************************      AND FINALLY     **************************/
	/*************************   declare RealHP<N>  **************************/
	/*************************************************************************/
	// OK. At this point we have the typelist:
	//	TypeListRealHP
	// which lists the consecutive types in the progressing precision ladder. Now we can use it to define RealHP<int> type.

#ifndef YADE_DISABLE_REAL_MULTI_HP // on older compilers or with older libraries, this can't work, cmake will detect such problems.
	template <int Level>
	using RealHP = typename std::conditional<
	        Level <= boost::mpl::size<detail::TypeListRealHP>::value,           // if Level <= than max size of TypeListRealHP
	        typename boost::mpl::at_c<detail::TypeListRealHP, Level - 1>::type, // then use the type from the typelist
	        detail::NthLevelRealHP<Level>>::type;                               // otherwise use the type constructed from MPFR or from boost::cpp_bin_float
	template <int Level>
	using ComplexHP = typename std::conditional<
	        IsWrapped<RealHP<Level>>,                                        // construct ComplexHP type
	        MakeWrappedComplex<UnderlyingComplexHP_fromReal<RealHP<Level>>>, // depending on whether it is wrapped or not
	        UnderlyingComplexHP_fromReal<RealHP<Level>>>::type;
#else
	//#warning "Using RealHP<…> is impossible on this system. Most likely the compiler is too old. Using a RealHP<1> for all possible precisions RealHP<N>."
	template <int Level> using RealHP = Real;
	template <int Level> using ComplexHP = Complex;
#endif
	// FIXME - notka ze str. …54 templates C++ - template <auto N> struct { … }; pozwala mieć zarówno typ jak i jego wartość. Ale chyba nadal jest nontype. tzn. musi mieć wartość?
	template <int N> using UnderlyingRealHPi = UnderlyingRealHP<RealHP<N>>; // his is to allow `int` template arguments  FIXME ? Or cleanup ?
	/*************************************************************************/
	/*************************   levelOfRealHP<…>   **************************/
	/*************************************************************************/
	namespace detail {
		// this one is to make sure that RealHP<levelOfRealHP<A>> == A
		template <typename A, typename boost::disable_if<boost::is_complex<A>, int>::type = 0>
		const constexpr bool VerifyRealHP = std::is_same<RealHP<CheckHPLevel<A>>, typename std::decay<A>::type>::value;
		template <typename A, typename boost::enable_if<boost::is_complex<A>, int>::type = 0>
		const constexpr bool VerifyComplexHP = std::is_same<ComplexHP<CheckHPLevel<typename A::value_type>>, typename std::decay<A>::type>::value;

		template <typename A, bool> struct DeferValueType2 {
			using type                       = A;
			static const constexpr bool isHP = std::is_same<RealHP<CheckHPLevel<A>>, typename std::decay<A>::type>::value;
		};
		template <typename A> struct DeferValueType2<A, true> {
			using type                       = typename A::value_type;
			static const constexpr bool isHP = std::is_same<ComplexHP<CheckHPLevel<typename A::value_type>>, typename std::decay<A>::type>::value;
		};

	}
	// levelOfRealHP<A> returns the N from RealHP<N>, if A is not a RealHP<N> type, then it fails to compile and returns compilation error.
	template <
	        typename A,
	        typename boost::disable_if<boost::is_complex<A>, int>::type                    = 0,
	        int Level                                                                      = detail::CheckHPLevel<A>,
	        typename boost::enable_if_c<1 <= Level and detail::VerifyRealHP<A>, int>::type = 0>
	const constexpr int levelOfRealHP = Level;
	template <
	        typename A,
	        typename boost::enable_if<boost::is_complex<A>, int>::type                        = 0,
	        int Level                                                                         = detail::CheckHPLevel<typename A::value_type>,
	        typename boost::enable_if_c<1 <= Level and detail::VerifyComplexHP<A>, int>::type = 0>
	const constexpr int levelOfComplexHP = Level;


	template <typename A, bool> struct DeferValueType {
		// FIXME - przykład w std::void_t rozpoznaje types that do have a nested ::type member
		// FIXME : https://en.cppreference.com/w/cpp/types/void_t
		using real_type                   = A;
		static const constexpr auto level = detail::CheckHPLevel<real_type>;
		using underlying                  = UnderlyingRealHP<real_type>;
	};
	template <typename A> struct DeferValueType<A, true> {
		using real_type                   = typename A::value_type;
		static const constexpr auto level = detail::CheckHPLevel<real_type>;
		using underlying                  = UnderlyingComplexHP_fromReal<real_type>;
	};

	// levelOfHP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	template <typename A> const constexpr int levelOfHP = DeferValueType<A, boost::is_complex<A>::value>::level;
	// IsHP      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	template <typename A>
	const constexpr bool IsHP = detail::DeferValueType2<A, boost::is_complex<A>::value>::isHP; //(VerifyRealHP<A> or VerifyComplexHP<A>);
	// UnderlyingHP
	template <typename A> using UnderlyingHP = typename DeferValueType<A, boost::is_complex<A>::value>::underlying;

	/*
	namespace HP {
		template <typename A> const constexpr int level = ::yade::math::levelOfRealHP<A>;
		template <typename A> using under               = typename ::yade::math::UnderlyingRealHP<A>;
	} // namespace HP
*/
	// SelectHigherHP typdefs ComplexHP if A or B is complex. Otherwise it typedefs the RealHP type.
	template <typename A, typename B, int LevelA = levelOfHP<A>, int LevelB = levelOfHP<B>, int MaxLevel = std::max(LevelA, LevelB)>
	using SelectHigherHP =
	        typename std::conditional<(boost::is_complex<A>::value or boost::is_complex<B>::value), ComplexHP<MaxLevel>, RealHP<MaxLevel>>::type;
} // namespace math

using math::ComplexHP;
using math::RealHP;


} // namespace yade

#endif

