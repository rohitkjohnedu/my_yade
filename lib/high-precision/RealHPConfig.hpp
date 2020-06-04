/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADE_REAL_HP_CONFIG_HPP
#define YADE_REAL_HP_CONFIG_HPP

#ifndef YADE_DISABLE_REAL_MULTI_HP
// Declare which precisions will be used in YADE for Eigen, CGAL and for minieigenHP (see file lib/high-precision/RealHPEigenCgal.hpp for details):
//	C++	: YADE_EIGENCGAL_HP  ↔ The numbers listed here will work in C++ for RealHP<N> in CGAL and Eigen. Rather cheap in compilation time.
//	Python	: YADE_MINIEIGEN_HP  ↔ These are exported to python. Expensive: each one makes compilation longer by 1 minute.
#define YADE_EIGENCGAL_HP (1)(2)(3)(4)(8)(10)(20)
#define YADE_MINIEIGEN_HP (1)(2)

// If you are doing some debugging and need more precisions or to access them from minieigenHP, then use e.g. this:
//#define YADE_EIGENCGAL_HP (1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(20)
//#define YADE_MINIEIGEN_HP YADE_EIGENCGAL_HP

#else // if cmake detects problems that RealHP<…> can't work, e.g. with gcc older than v9.2.1
#define YADE_EIGENCGAL_HP (1)
#define YADE_MINIEIGEN_HP (1)
#endif

#include <lib/high-precision/Real.hpp>
#include <array>
#include <boost/mpl/vector_c.hpp>
#include <boost/python.hpp>

namespace yade {
namespace math {
	struct RealHPConfig {
		// set how many RealHP<N> types are provided for Eigen, CGAL and Minieigen in file lib/high-precision/RealHPEigenCgal.hpp by YADE_EIGENCGAL_HP , YADE_MINIEIGEN_HP:
		static const constexpr auto sizeEigenCgal = BOOST_PP_SEQ_SIZE(YADE_EIGENCGAL_HP);
		static const constexpr auto sizeMinieigen = BOOST_PP_SEQ_SIZE(YADE_MINIEIGEN_HP);

		// make a runtime iterable array (runtime variable names start with lowerCase)
		static const constexpr std::array<int, sizeEigenCgal> supportedByEigenCgal { BOOST_PP_SEQ_ENUM(YADE_EIGENCGAL_HP) };
		static const constexpr std::array<int, sizeMinieigen> supportedByMinieigen { BOOST_PP_SEQ_ENUM(YADE_MINIEIGEN_HP) };

		// make a compile-time iterable mpl::vector_c (type names start with UpperCase)
		using SupportedByEigenCgal = boost::mpl::vector_c<int, BOOST_PP_SEQ_ENUM(YADE_EIGENCGAL_HP)>;
		using SupportedByMinieigen = boost::mpl::vector_c<int, BOOST_PP_SEQ_ENUM(YADE_MINIEIGEN_HP)>;

		// export to python
		static inline boost::python::tuple getSupportedByEigenCgal() { return boost::python::make_tuple(BOOST_PP_SEQ_ENUM(YADE_EIGENCGAL_HP)); }
		static inline boost::python::tuple getSupportedByMinieigen() { return boost::python::make_tuple(BOOST_PP_SEQ_ENUM(YADE_MINIEIGEN_HP)); }

		// returns number of decimal digits for compile-time N of RealHP<N>
		template <int N> static const constexpr auto digits10 = std::numeric_limits<RealHP<N>>::digits10;
		// returns number of decimal digits for runtime N of RealHP<N>
		static int getDigits10(int N);

		// register this class to python
		static void pyRegister();

		// how many extra digits to use when converting to decimal srings
		static int extraStringDigits10;
	};
} // namespace math
} // namespace yade

#endif

