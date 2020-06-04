/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <lib/high-precision/RealHPConfig.hpp>
#include <lib/high-precision/RealIO.hpp>

namespace yade {
namespace math {

	int RealHPConfig::extraStringDigits10 { 1 };

	int RealHPConfig::getDigits10(int N)
	{
		// 5 is the largest length of RealHPLadder<…>. If more were added, and precision were not the multiplies of digits10*N
		// then the python test will quickly catch that problem. Then more cases will be needed to add to this switch.
		static_assert(
		        boost::mpl::size<::yade::math::RealHPLadder>::value <= 5,
		        "More types were added in RealHP.hpp, please adjust this switch(…) accordingly.");
		switch (N) {
			case 1: return digits10<1>;
			case 2: return digits10<2>;
			case 3: return digits10<3>;
			case 4: return digits10<4>;
			case 5: return digits10<5>;
			default: return digits10<1> * N; // this formula is used by NthLevel in lib/high-precision/RealHP.hpp
		}
	}

	void RealHPConfig::pyRegister()
	{
		namespace py                            = ::boost::python;
		py::scope here                          = py::class_<RealHPConfig>("RealHPConfig");
		py::scope().attr("extraStringDigits10") = extraStringDigits10;
		py::def("getSupportedByEigenCgal",
		        getSupportedByEigenCgal,
		        R"""(:return: the ``tuple`` containing N from RealHP<N> precisions supported by Eigen and CGAL)""");
		py::def("getSupportedByMinieigen",
		        getSupportedByMinieigen,
		        R"""(:return: the ``tuple`` containing N from RealHP<N> precisions supported by minieigenHP)""");
		py::def("getDigits10", getDigits10, (py::arg("N")), R"""(:return: the ``int`` representing numeric_limits digits10 of RealHP<N>)""");
#if (GCC_VERSION < 90201)
#ifndef YADE_DISABLE_REAL_MULTI_HP
#warning "RealHP<…> won't work on this system, cmake sets YADE_DISABLE_REAL_MULTI_HP to use RealHP<1> for all precisions RealHP<N>. Also you can try -O0 flag."
// see file lib/high-precision/RealHP.hpp line: 'template <int Level> using RealHP    = Real;'
#endif
		// When using gcc older than 9.2.1 it is not possible for RealHP<N> to work. Without optimizations -O0 it can work, except for float128.
		// If YADE_DISABLE_REAL_MULTI_HP is set, then RealHP<1> is used in place of all possible precisions RealHP<N> : see file RealHP.hpp for this setting.
		// So this is for local testing only. With flag -O0 most of RealHP<…> works, except for float128 which is always segfaulting.
		py::scope().attr("isFloat128Broken") = true;
#else
		py::scope().attr("isFloat128Broken") = false;
#endif
	}

} // namespace math
} // namespace yade

