/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/*

This file provides explicit instantinations for RealHP<N> types. Before we switch to C++20 they are unfortunately necessary.

There are two ways to avoid these macors:

1) modify Eigen library to change the main template declaration from

	template<typename>
	struct NumTraits;

into

	template<typename, typename=void>
	struct NumTraits;

then we could use the second typename to enable/disable our template specialization, by writing:

	template <typename Rr>
	struct NumTraits <Rr, typename std::enable_if<::yade::math::IsHP<Rr>>::type> : public NumTraitsHP<::yade::math::levelOfRealHP<Rr>> {};

I checked locally that modification of Eigen library fixes the template specializations and removes macros.

2) wait until we move yade to C++20 and then use the https://en.cppreference.com/w/cpp/language/constraints to enable/disable our
   template specializations of NumTraitsRealHP (which can then be renamed to Eigen::NumTraits) using the `requires` and `using`
   C++20 keywords, explained in section "Type requirements". The classes can then be renamed back to original names:
	NumTraitsRealHP				→ Eigen::NumTraits
	RealHP_Is_valid				→ CGAL::Is_valid
	RealHP_Algebraic_structure_traits	→ CGAL::Algebraic_structure_traits 
	RealHP_embeddable_traits		→ CGAL::Real_embeddable_traits

*/

#ifndef YADE_EXPLICIT_REAL_HP_HPP
#define YADE_EXPLICIT_REAL_HP_HPP

#if ((not defined(YADE_REAL_MATH_NAMESPACE)) or (not defined(YADE_EIGEN_SUPPORT_REAL_HP)) or (defined(YADE_CGAL) and (not defined(YADE_CGAL_SUPPORT_REAL_HP))))
#error "This file cannot be included alone, include Real.hpp instead"
#endif

namespace forCtags {
struct ExplicitRealHP {
}; // for ctags
}

// The float and double are supported by default in Eigen and CGAL, so they have to be skipped.
#define YADE_REAL_HP_ALL_ADD_EXPLICIT_SUPPORT_FOR(name) name(1) name(2) name(3) name(4) name(5) name(6) name(7) name(8) name(9) name(10)
#if (YADE_REAL_BIT >= 80)
#define YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR(name) YADE_REAL_HP_ALL_ADD_EXPLICIT_SUPPORT_FOR(name)
#elif (YADE_REAL_BIT >= 64)
#define YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR(name) name(2) name(3) name(4) name(5) name(6) name(7) name(8) name(9) name(10)
#elif (YADE_REAL_BIT >= 32)
#define YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR(name) name(3) name(4) name(5) name(6) name(7) name(8) name(9) name(10)
#endif

namespace Eigen {

YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR(YADE_EIGEN_SUPPORT_REAL_HP)
#undef YADE_EIGEN_SUPPORT_REAL_HP

} // namespace Eigen

#ifdef YADE_CGAL
namespace CGAL {

YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR(YADE_CGAL_SUPPORT_REAL_HP)
#undef YADE_CGAL_SUPPORT_REAL_HP

} // namespace CGAL
#endif // YADE_CGAL

#undef YADE_REAL_HP_ADD_EXPLICIT_SUPPORT_FOR

#endif

