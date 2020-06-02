/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <lib/high-precision/RealHPInfo.hpp>

namespace yade {
namespace math {

	int RealHPInfo::getRealHPDigits10(int N)
	{
		// 5 is the largest length of TypeListRealHP<…>. If more were added, and precision were not the multiplies of digits10*N
		// then the python test will quickly catch that problem. And more cases will be needed to add to this switch.
		static_assert(
		        boost::mpl::size<::yade::math::detail::TypeListRealHP>::value <= 5,
		        "More types were added in RealHP.hpp, please adjust this switch(…) accordingly.");
		switch (N) {
			case 1: return getDigits10<1>;
			case 2: return getDigits10<2>;
			case 3: return getDigits10<3>;
			case 4: return getDigits10<4>;
			case 5: return getDigits10<5>;
			default: return getDigits10<1> * N; // this formula is used by NthLevel in lib/high-precision/RealHP.hpp
		}
	}
} // namespace math
} // namespace yade

