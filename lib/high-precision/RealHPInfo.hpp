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
		// set how many RealHP<N> types are provided for C++ and Python in file lib/high-precision/ExplicitRealHP.hpp by YADE_EXPLICIT_REAL_HP , YADE_PYTHON_REAL_HP:
		static const constexpr auto sizeCpp    = BOOST_PP_SEQ_SIZE(YADE_EXPLICIT_REAL_HP);
		static const constexpr auto sizePython = BOOST_PP_SEQ_SIZE(YADE_PYTHON_REAL_HP);

		// make a runtime iterable array (runtime variable names start with lowerCase)
		static const constexpr std::array<int, sizeCpp>    supportedByCpp { BOOST_PP_SEQ_ENUM(YADE_EXPLICIT_REAL_HP) };
		static const constexpr std::array<int, sizePython> supportedByPython { BOOST_PP_SEQ_ENUM(YADE_PYTHON_REAL_HP) };

		// make a compile-time iterable mpl::vector_c (type names start with UpperCase)
		using SupportedByCpp    = boost::mpl::vector_c<int, BOOST_PP_SEQ_ENUM(YADE_EXPLICIT_REAL_HP)>;
		using SupportedByPython = boost::mpl::vector_c<int, BOOST_PP_SEQ_ENUM(YADE_PYTHON_REAL_HP)>;

		// export to python
		static inline boost::python::tuple getRealHPSupportedByCpp() { return boost::python::make_tuple(BOOST_PP_SEQ_ENUM(YADE_EXPLICIT_REAL_HP)); }
		static inline boost::python::tuple getRealHPSupportedByPython() { return boost::python::make_tuple(BOOST_PP_SEQ_ENUM(YADE_PYTHON_REAL_HP)); }

		// returns number of decimal digits for compile-time N of RealHP<N>
		template <int N> static const constexpr auto getDigits10 = std::numeric_limits<RealHP<N>>::digits10;
		// returns number of decimal digits for runtime N of RealHP<N>
		static int getRealHPDigits10(int N);
	};
} // namespace math
} // namespace yade

#endif

