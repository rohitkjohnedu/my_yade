/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <lib/high-precision/RealHPInfo.hpp>
#include <lib/high-precision/RealIO.hpp>

namespace yade {
namespace math {

	int RealHPInfo::getDigits10(int N)
	{
		// 5 is the largest length of TypeListRealHP<…>. If more were added, and precision were not the multiplies of digits10*N
		// then the python test will quickly catch that problem. And more cases will be needed to add to this switch.
		static_assert(
		        boost::mpl::size<::yade::math::detail::TypeListRealHP>::value <= 5,
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

	void RealHPInfo::pyRegister()
	{
		namespace py                          = ::boost::python;
		py::scope here                        = py::class_<RealHPInfo>("RealHPInfo");
		py::scope().attr("extraStringDigits") = ::yade::math::extraDigits10NecessaryForStringRepresentation;
		py::def("getSupportedByEigenCgal", getSupportedByEigenCgal);
		py::def("getSupportedByMinieigen", getSupportedByMinieigen);
		py::def("getDigits10", getDigits10, (py::arg("N")));
#if __GNUC__ < 9
		// this is for local testing only. It's here because on older compilers using flag -O0 most of RealHP<…> works, except for float128 which is segfaulting. The -O0 makes it useless anyway.
		py::scope().attr("isFloat128Broken") = true;
#else
		py::scope().attr("isFloat128Broken") = false;
#endif
	}
} // namespace math
} // namespace yade

