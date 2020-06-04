/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// Separate header for all Real IO operations.

// Unfortunately std::to_string cannot be relied upon. This is why: https://en.cppreference.com/w/cpp/string/basic_string/to_string
//
// Hence following the boost multiprecision recommendation we will use stringstream as general conversion method:
//   → https://www.boost.org/doc/libs/1_72_0/doc/html/boost_lexical_cast.html
//
// quote: "For more involved conversions, such as where precision or formatting need tighter control than is offered by
// the default behavior of lexical_cast, the conventional std::stringstream approach is recommended. Where the conversions
// are numeric to numeric, boost::numeric_cast may offer more reasonable behavior than lexical_cast."
//

#pragma once

#include <lib/base/Logging.hpp>
#include <lib/high-precision/Real.hpp>
#include <boost/integer/static_log2.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>

namespace yade {
namespace math {
	/* the extraDigits10NecessaryForStringRepresentation is to make sure that there are no conversion errors in the last bit.
	   a quick python example which shows the 'cutting' of last digits.

# This one demostrates that `double` used by python has just 53 bits of precision:

for a in range(128): print(str(a).rjust(3,' ')+": "+str(1+1./pow(2.,a)))

# This one shows the 'true' values:

import mpmath; mpmath.mp.dps=200;
for a in range(128): print(str(a).rjust(3,' ')+": "+str(mpmath.mpf(1)+mpmath.mpf(1)/pow(mpmath.mpf(2),mpmath.mpf(a))))

# This one shows the actual 'Real' precision used in yade. To achieve this the mth.max(…,…) are called to force the numbers
# to pass through C++, instead of letting mpmath to calculate this, so for example we can see that float128 has 113 bits.
# Also this test was used to verify the value for extraDigits10NecessaryForStringRepresentation as well as the formula given
# in IEEE Std 754-2019 Standard for Floating-Point Arithmetic: Pmin (bf) = 1 + ceiling( p × log10(2)), where p is the number of significant bits in bf

from yade import math as mth
for a in range(128): print(str(a).rjust(3,' ')+": "+str(mth.max(0,mth.max(0,1)+mth.max(0,1)/mth.pow(mth.max(0,2),a))))


# Hmm maybe turn this into an external parameter? Configurable from python? And write in help "use 1 to extract results and avoid fake sense of more precision,
# use 4 or more to have nubers which will convert exactly in both directions mpmath ↔ string ↔ C++.".
# For now it is in yade.math.extraDigits10NecessaryForStringRepresentation
*/

	//////////////////// XXX:  	std::streamsize max_digits10 = 2 + std::numeric_limits<double>::digits * 30103UL / 100000UL;
	//                                                                                                       ↑↑↑
	//                                                                                           in boost they used this trick to obtain 0.30103

	constexpr const double log_10_2 = 0.301029995663981195213738;

	template <int n> struct Try;

	template <> struct Try<1> {
		static const constexpr double result { 1.0 };
	};

	template <int n> struct Try {
		static const constexpr double result { 1.0 / (double)n * Try<1>::result };
	};

	template <typename T> constexpr int32_t constexprCeiling(T num)
	{
		return (static_cast<T>(static_cast<int32_t>(num)) == num) ? static_cast<int32_t>(num) : static_cast<int32_t>(num) + ((num > 0) ? 1 : 0);
	}

	// These formulas are from the IEEE Std 754™-2019, IEEE Standard for Floating-Point Arithmetic, DOI:10.1109/IEEESTD.2019.8766229, pages 24 and 46
	constexpr const auto realBit         = YADE_REAL_BIT;
	constexpr const auto realDec         = YADE_REAL_DEC;
	constexpr const auto significandIeee = realBit - (4 * ::boost::static_log2<realBit>::value - 13);
	constexpr const auto exponentIeee    = realBit - significandIeee;
	constexpr const auto reqDigits10
	        = 1 + constexprCeiling(Try<significandIeee>::result /* * 10000000 / 33219281 */ /*0.301029995663981195213738*/ /*log_10_2*/);
	constexpr const auto extraDigits10NecessaryForStringRepresentation = 1;
	//static_assert((reqDigits10 - 3) == std::numeric_limits<Real>::digits10, "IEEE error.");

	// guaranteed maximum precision
	template <typename RC, int Level = levelOfHP<RC>> inline std::string toStringHP(const RC& val)
	{
		constexpr const auto digs1 = std::numeric_limits<RealOf<RC>>::digits10 + ::yade::math::extraDigits10NecessaryForStringRepresentation;
		std::ostringstream   ss;
		ss << std::setprecision(digs1) << val;
		return ss.str();
	};

	// These are just an inline convenience functions. They are the same as using std::stringstream.
	template <typename Rr, int Level = levelOfRealHP<Rr>> inline Rr fromStringRealHP(const std::string& st)
	{
		// ::digits is number of used bits (base 2), ::digits10 is base 10. YADE_REAL_BIT==80 corresponds to long double which has 64 bit fraction part.
		// The idea here is to use stringstream for anything larger than long double, and use lexical_cast for long double and smaller types.
		// This is to ensure that Inf, NaN are handled correctly, because lexical_cast from boost fixed the stringstream problems up to long double type.
		if (std::numeric_limits<Rr>::digits > std::numeric_limits<long double>::digits) {
			Rr                ret;
			std::stringstream s { st };
			s >> ret;
			return ret;
		} else {
			return boost::lexical_cast<math::UnderlyingHP<Rr>>(st);
		}
	};


	template <typename Cc, int Level = levelOfComplexHP<Cc>> inline Cc fromStringComplexHP(const std::string& st)
	{
		if (std::numeric_limits<Cc>::digits <= std::numeric_limits<long double>::digits) {
			// NOTE: if reading complex is needed, the lack of standard approach to nonfinite numbers will need a workaround here.
			//       fortunately ArbitraryComplex_from_python does not use this. It uses fromStringReal separately for each component.
			//       and we usually deal only with input from python. So that's good. And probably we will never see following message:
			LOG_NOFILTER(R"""(Warning: Reading complex number "(nan,nan)" or "(inf,0)" is not handled correctly by stringstream)""");
		}
		Cc                ret;
		std::stringstream s { st };
		s >> ret;
		return ret;
	};

	inline std::string toString(const Real& val) { return toStringHP<Real>(val); }
	inline std::string toString(const Complex& val) { return toStringHP<Complex>(val); }
	inline Real        fromStringReal(const std::string& st) { return fromStringRealHP<Real>(st); }
	inline Complex     fromStringComplex(const std::string& st) { return fromStringComplexHP<Complex>(st); }

}
}

namespace Eigen {
/*
 These operator<< specialisations have to be in Eigen namespace because ::boost::log
 namespace can perform ADL lookup only into ::std and ::Eigen namespaces when
 searching for an overlaod of std::ostream& operator<<(std::ostream& os,const ::yade::Vector3<Scalar>& v)
 and others. This is because our ::yade::Vector* typedefs in fact resolve into types from ::Eigen.

 So without putting these operator<< inside ::Eigen namespace the default Eigen operator<<(…)
 are used by ::boost::log

 This is clearly a bug. However I am not sure what is to blame:
 (1) The ADL not flexible enough, and ignoring ::yade entirely?
 (2) The ::boost::log, because the ADL allows it to check only ::std and ::Eigen and has no opportunity
     to check ::yade?
 (3) The Logger.hpp, because writing some variation of 'namespace boost{namespace log{ using ::yade::operator<<; }}'
     in Logger.hpp probably could also make it work? But Logger.hpp has no idea about Eigen at all. Putting it
     there is causing an #include-dependency cycle.

 This interesting problem was brought to you by four namespaces:

                                    ( g l o b a l   n a m e s a p a c e )
                                      |          |         |          |
                             ::boost::log     ::Eigen    ::std       ::yade
*/

template <class Scalar>::std::ostream& operator<<(::std::ostream& os, const ::yade::Vector2<Scalar>& v)
{
	os << v.x() << " " << v.y();
	return os;
}

template <class Scalar>::std::ostream& operator<<(::std::ostream& os, const ::yade::Vector3<Scalar>& v)
{
	os << v.x() << " " << v.y() << " " << v.z();
	return os;
}

template <class Scalar>::std::ostream& operator<<(::std::ostream& os, const ::yade::Vector6<Scalar>& v)
{
	os << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5];
	return os;
}

template <class Scalar>::std::ostream& operator<<(::std::ostream& os, const ::Eigen::Quaternion<Scalar>& q)
{
	os << q.w() << " " << q.x() << " " << q.y() << " " << q.z();
	return os;
}
}
