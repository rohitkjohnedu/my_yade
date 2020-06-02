/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADE_EXPLICIT_REAL_HP_HPP
#define YADE_EXPLICIT_REAL_HP_HPP

#include <boost/preprocessor.hpp>

// RealHP<…> can't work with older gcc, cmake will detect this.
#ifndef YADE_DISABLE_REAL_MULTI_HP

/*

This file provides explicit instantinations for RealHP<N>, Vector3rHP<N> etc. types. Before we switch to C++20 this file is unfortunately necessary.

One cannot "just use Vector3rHP<123>", and be happy, because Eigen and CGAL are not flexible enough in template specialization mechanisms.
The RealHP<123> just works, but for Vector3rHP<123> this file is necessary.

The current solution to this problem is to set the list of supported numbers below (separately for C++ and Python):
	C++	: YADE_EXPLICIT_REAL_HP  ↔ The numbers listed here will work in C++ for RealHP<N> in CGAL and Eigen. Rather cheap in compilation time.
	Python	: YADE_PYTHON_REAL_HP    ↔ These are exported to python. Expensive: each one makes compilation longer by 1 minute.

Caution: trying to use an unregistered for python Vector3rHP<N> type in YADE_CLASS_BASE_DOC_ATTRS_* to export value to Python will cause problems.
         however it is safe (and intended) to use them in the C++ calculations in critical sections of code, without exporting them to python.
*/

#define YADE_EXPLICIT_REAL_HP (1)(2)(3)(4)(5)(6)(7)(8)(9)(10)
//#define YADE_PYTHON_REAL_HP (1)(2)(3)(4)

// Python: if you are doing some debugging, and need to access from python all the precisions that are used in C++, then instead of above, use this:
#define YADE_PYTHON_REAL_HP YADE_EXPLICIT_REAL_HP

#else // on older compilers or with older libraries, this can't work, cmake will detect such problems. In this case only RealHP<1> is instantinated.
#define YADE_EXPLICIT_REAL_HP (1)
#endif

/*
There are two ways to avoid these macros:

1) modify Eigen library to change the main template declaration from

	template<typename>
	struct NumTraits;

   into

	template<typename, typename=void>
	struct NumTraits;

   then we could use the second typename to enable/disable our template specialization, by writing:

	template <typename Rr>
	struct NumTraits <Rr, typename std::enable_if<::yade::math::IsHP<Rr>>::type> : public NumTraitsHP<::yade::math::levelOfRealHP<Rr>> {};

   Do the same with CGAL.
   I checked locally that modification of Eigen library fixes the template specializations and removes all these macros.

2) wait until we move yade to C++20 and then use the https://en.cppreference.com/w/cpp/language/constraints to enable/disable our
   template specializations of NumTraitsRealHP (which can then be renamed to Eigen::NumTraits) using the `requires` and `using`
   C++20 keywords, explained in section "Type requirements". The classes can then be renamed back to original names:

	NumTraitsRealHP				→ Eigen::NumTraits
	RealHP_Is_valid				→ CGAL::Is_valid
	RealHP_Algebraic_structure_traits	→ CGAL::Algebraic_structure_traits 
	RealHP_embeddable_traits		→ CGAL::Real_embeddable_traits

*/

#if ((not defined(YADE_REAL_MATH_NAMESPACE)) or (not defined(YADE_EIGEN_SUPPORT_REAL_HP)) or (defined(YADE_CGAL) and (not defined(YADE_CGAL_SUPPORT_REAL_HP))))
#error "This file cannot be included alone, include Real.hpp instead"
#endif

namespace forCtags {
struct ExplicitRealHP {
}; // for ctags
}

#define YADE_HP_PARSE_ONE(r, name, levelHP) name(levelHP)
// This macro ↓ is used in AliasCGAL.hpp, that code could be put here, but this would make compilation unnecessarily longer.
#define YADE_HP_RUN_MACRO(name) BOOST_PP_SEQ_FOR_EACH(YADE_HP_PARSE_ONE, name, YADE_EXPLICIT_REAL_HP) // it just creates: name(1) name(2) name(3) ....

/*
 * Macro YADE_HP_PYTHON_REGISTER generates an assembly code for a template instantination. It is used in files:
 *
 * py/high-precision/_ExposeBoxes.cpp        py/high-precision/_ExposeConverters.cpp    py/high-precision/_ExposeQuaternion.cpp
 * py/high-precision/_ExposeComplex1.cpp     py/high-precision/_ExposeMatrices1.cpp     py/high-precision/_ExposeVectors1.cpp
 * py/high-precision/_ExposeComplex2.cpp     py/high-precision/_ExposeMatrices2.cpp     py/high-precision/_ExposeVectors2.cpp
 *
 * Each number in YADE_PYTHON_REAL_HP, defined above,  makes compilation longer by 1 minute. So put there only the ones which are really needed to be accessed from python.
 */
#define YADE_HP_PY_EIGEN(r, name, levelHP) template void name<levelHP>();
#define YADE_HP_PYTHON_REGISTER(name) BOOST_PP_SEQ_FOR_EACH(YADE_HP_PY_EIGEN, name, YADE_PYTHON_REAL_HP) // instatinate templates for name<1>, name<2>, etc …

/*
 * The float and double are supported by default in Eigen and CGAL, so they have to be skipped.
 * This macro FOR loop 'BOOST_PP_SEQ_FOR_EACH' skips registration of those that are already registered.
 */
#define YADE_SKIP_ARG(arg)
#define YADE_HP_PARSE_SEQUENCE(r, data, levelHP)                                                                                                               \
	BOOST_PP_IF(                                                                                                                                           \
	        BOOST_PP_GREATER_EQUAL(levelHP, /* skip 'below' this number */ BOOST_PP_SEQ_ELEM(1, data)),                                                    \
	        /* execute macro with this 'name'   */ BOOST_PP_SEQ_ELEM(0, data),                                                                             \
	        /* skip the already registered one  */ YADE_SKIP_ARG)                                                                                          \
	(levelHP)
#define YADE_HP_RUN_MACRO_FROM_LEVEL(name, below)                                                                                                              \
	BOOST_PP_SEQ_FOR_EACH(                                                                                                                                 \
	        YADE_HP_PARSE_SEQUENCE, (/* the macro 'name' to be executed */ name)(/* skip numbers 'below' this value */ below), YADE_EXPLICIT_REAL_HP)

#if (YADE_REAL_BIT >= 80)
#define YADE_HP_RUN_EXPLICIT_MACRO(name) YADE_HP_RUN_MACRO(name) //               it just creates: name(1) name(2) name(3) .... using YADE_EXPLICIT_REAL_HP
#elif (YADE_REAL_BIT >= 64)
#define YADE_HP_RUN_EXPLICIT_MACRO(name) YADE_HP_RUN_MACRO_FROM_LEVEL(name, 2) // it just creates:         name(2) name(3) ....
#elif (YADE_REAL_BIT >= 32)
#define YADE_HP_RUN_EXPLICIT_MACRO(name) YADE_HP_RUN_MACRO_FROM_LEVEL(name, 3) // it just creates:                 name(3) ....
#endif

namespace Eigen {

// The YADE_EIGEN_SUPPORT_REAL_HP is provided from file lib/high-precision/EigenNumTraits.hpp
YADE_HP_RUN_EXPLICIT_MACRO(YADE_EIGEN_SUPPORT_REAL_HP)
#undef YADE_EIGEN_SUPPORT_REAL_HP

} // namespace Eigen

#ifdef YADE_CGAL
namespace CGAL {

// The YADE_CGAL_SUPPORT_REAL_HP is provided from file lib/high-precision/CgalNumTraits.hpp
YADE_HP_RUN_EXPLICIT_MACRO(YADE_CGAL_SUPPORT_REAL_HP)
#undef YADE_CGAL_SUPPORT_REAL_HP

} // namespace CGAL
#endif // YADE_CGAL

#undef YADE_HP_RUN_EXPLICIT_MACRO

#endif

