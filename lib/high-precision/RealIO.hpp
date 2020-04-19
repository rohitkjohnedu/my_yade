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

#include <lib/high-precision/Real.hpp>
#include <sstream>
#include <string>

#if (YADE_REAL_BIT <= 80)
#include <lib/base/Logging.hpp>
#include <boost/lexical_cast.hpp>
#endif

namespace yade {
namespace math {
	// guaranteed maximum precision
	inline std::string toString(const Real& val)
	{
		constexpr const auto digs1 = std::numeric_limits<Real>::digits10 + 1;
		std::ostringstream   ss;
		ss << std::setprecision(digs1) << val;
		return ss.str();
	};

	inline std::string toString(const Complex& val)
	{
		std::ostringstream   ss;
		constexpr const auto digs1 = std::numeric_limits<Real>::digits10 + 1;
		ss << std::setprecision(digs1) << val;
		return ss.str();
	};

	// These are just an inline convenience functions. They are the same as using std::stringstream.
	inline Real fromStringReal(const std::string& st)
	{
#if (YADE_REAL_BIT > 80)
		Real              ret;
		std::stringstream s { st };
		s >> ret;
		return ret;
#else
		return boost::lexical_cast<math::UnderlyingReal>(st);
#endif
	};

	inline Complex fromStringComplex(const std::string& st)
	{
#if (YADE_REAL_BIT <= 80)
		// NOTE: if reading complex is needed, the lack of standard approach to nonfinite numbers will need a workaround here.
		//       fortunately ArbitraryComplex_from_python does not use this. It uses fromStringReal separately for each component.
		//       and we usually deal only with input from python. So that's good. And probably we will never see following message:
		LOG_NOFILTER(R"""(Warning: Reading complex number "(nan,nan)" or "(inf,0)" is not handled correctly by stringstream)""");
#endif
		Complex           ret;
		std::stringstream s { st };
		s >> ret;
		return ret;
	};

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
