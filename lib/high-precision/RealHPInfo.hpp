/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADE_REAL_HP_INFO_HPP
#define YADE_REAL_HP_INFO_HPP

#include <lib/high-precision/Real.hpp>
#include <array>
#include <boost/mpl/vector_c.hpp>
#include <boost/python.hpp>

namespace yade {
namespace math {
	struct RealHPInfo {
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

		// registers itself to python
		static void pyRegister();
	};
} // namespace math
} // namespace yade

#endif

