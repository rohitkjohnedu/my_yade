/*************************************************************************
*  2019 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADE_REAL_MATH_NAMESPACE
#error "This file cannot be included alone, include Real.hpp instead"
#endif

#ifndef YADE_EIGEN_NUM_TRAITS_HPP
#define YADE_EIGEN_NUM_TRAITS_HPP

// compare this file with /usr/include/eigen3/Eigen/src/Core/NumTraits.h
// http://eigen.tuxfamily.org/dox/TopicCustomizing_CustomScalar.html
// http://eigen.tuxfamily.org/dox-3.2/TopicCustomizingEigen.html

#include <Eigen/Core>
#include <boost/math/constants/constants.hpp>

/*************************************************************************/
/*************************        Real          **************************/
/*************************************************************************/

namespace Eigen {

template <typename> struct EigenCostRealHP {
	// FIXME - make sure that these enums are properly recognized; does _math.cpp export them in expose_storage_ordering() ?
	// FIXME - add test for that
	enum { ReadCost = Eigen::HugeCost, AddCost = Eigen::HugeCost, MulCost = Eigen::HugeCost };
};

template <> struct EigenCostRealHP<boost::float_fast80_t> {
	enum { ReadCost = 1, AddCost = 1, MulCost = 1 };
};
#ifdef BOOST_MP_FLOAT128_HPP
template <> struct EigenCostRealHP<boost::multiprecision::float128> {
	enum { ReadCost = 1, AddCost = 2, MulCost = 2 };
};
#endif

/*
template <typename> struct EigenCostRealHP {
// FIXME - make sure that these enums are properly recognized; does _math.cpp export them in expose_storage_ordering() ?
// FIXME - add test for that
	static const constexpr auto ReadCost = Eigen::HugeCost;
	static const constexpr auto AddCost  = Eigen::HugeCost;
	static const constexpr auto MulCost  = Eigen::HugeCost;
};

template <> struct EigenCostRealHP<boost::float_fast80_t>
{
	static const constexpr auto ReadCost = 1;
	static const constexpr auto AddCost  = 1;
	static const constexpr auto MulCost  = 1;
};
#ifdef BOOST_MP_FLOAT128_HPP
template <> struct EigenCostRealHP<boost::multiprecision::float128>
{
	static const constexpr auto ReadCost = 1;
	static const constexpr auto AddCost  = 2;
	static const constexpr auto MulCost  = 2;
};
#endif
*/

// signature of general template from Eigen headers.
template<class> struct NumTraits;
// NOTE: Don't include this file for float, double, long double. Otherwise you will get errors like:
// error: redefinition of ‘struct Eigen::NumTraits<long double>’
// note: previous definition of ‘struct Eigen::NumTraits<long double>’ in /usr/include/eigen3/Eigen/src/Core/NumTraits.h

// FIXME  ↓ Remove this comment
// trudność:
//   N == 1 to może być float128 lub float(dla którego już istnieje przeładowanie)
//   RealHP<1> to może być float. Więc taka dedukcja nie jest jasna.
template <int N>
struct NumTraitsHP : GenericNumTraits<::yade::RealHP<N>> { // NOTE: Don't include this file for float, double, long double.
	typedef ::yade::RealHP<N> Real; // XXX   Real == RealHP<N>, rename locally maybe?
	typedef ::yade::RealHP<N> NonInteger;
	typedef ::yade::RealHP<N> Nested;

	enum { IsInteger             = 0,
	       IsSigned              = 1,
	       IsComplex             = 0,
	       RequireInitialization = 1,
// FIXME - ReadCost etc → ReadCostHP<…>, because it depends on N.
	       ReadCost              = ::Eigen::EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::ReadCost,
	       AddCost               = ::Eigen::EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::AddCost,
	       MulCost               = ::Eigen::EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::MulCost };

	static constexpr long get_default_prec = std::numeric_limits<Real>::digits;

	// http://cs.swan.ac.uk/~csoliver/ok-sat-library/internet_html/doc/doc/Mpfr/3.0.0/mpfr.html/Exception-Related-Functions.html
	// https://tspiteri.gitlab.io/gmp-mpfr-sys/mpfr/MPFR-Interface.html
	static inline Real highest(long = get_default_prec) { return std::numeric_limits<Real>::max(); }
	static inline Real lowest(long = get_default_prec) { return std::numeric_limits<Real>::lowest(); }

	// Constants
	static inline Real Pi(long = get_default_prec) { return boost::math::constants::pi<Real>(); }
	static inline Real Euler(long = get_default_prec) { return boost::math::constants::euler<Real>(); }
	static inline Real Log2(long = get_default_prec) { return ::yade::math::log(Real(2)); }
	static inline Real Catalan(long = get_default_prec) { return boost::math::constants::catalan<Real>(); }

	static inline Real epsilon(long = get_default_prec) { return std::numeric_limits<Real>::epsilon(); }
	static inline Real epsilon(const Real&) { return std::numeric_limits<Real>::epsilon(); }
	static inline Real smallest_positive() { return std::numeric_limits<Real>::min(); }
	static inline int  digits10(long = get_default_prec) { return std::numeric_limits<Real>::digits10; }
	static inline int  digits10(const Real&) { return std::numeric_limits<Real>::digits10; }
	static inline Real dummy_precision() { return epsilon() * ::yade::math::pow(Real(10), digits10() / Real(10)); }
};

namespace internal {
	//template <typename Rr> inline typename boost::enable_if_c<::yade::math::IsHP<Rr>, Rr>::type random<Rr>() { return ::yade::math::randomHP<::yade::math::levelOfRealHP<Rr>>(); }
	template <int N> inline ::yade::math::RealHP<N> random() { return ::yade::math::randomHP<N>(); }
	// FIXME - maybe eveywhere use this:
	// template <typename Rr> inline typename boost::enable_if_c<::yade::math::IsHP<Rr>, bool>::type
	// ???
	template <int N> inline ::yade::math::RealHP<N> random(const ::yade::math::RealHP<N>& a, const ::yade::math::RealHP<N>& b)
	{
	// FIXME - make sure these functions are called, I am no so sure that <int N> is properly resolved.
		return a + (b - a) * ::yade::math::random01HP<N>();
	}
	template <int N> inline bool isMuchSmallerThan(const ::yade::math::RealHP<N>& a, const ::yade::math::RealHP<N>& b, const ::yade::math::RealHP<N>& eps)
	{
		return ::yade::math::abs(a) <= ::yade::math::abs(b) * eps;
	}
	template <typename Rr> inline typename boost::enable_if_c<::yade::math::IsHP<Rr>, bool>::type
	isEqualFuzzy(const Rr& a, const Rr& b, const Rr& eps)
	//template <int N> inline bool isEqualFuzzy(const ::yade::math::RealHP<N>& a, const ::yade::math::RealHP<N>& b, const ::yade::math::RealHP<N>& eps)
	{
		return ::yade::math::abs(a - b) <= eps;
	}
	template <int N> inline bool isApprox(const ::yade::math::RealHP<N>& a, const ::yade::math::RealHP<N>& b, const ::yade::math::RealHP<N>& eps) { return isEqualFuzzy(a, b, eps); }
	template <int N> inline bool isApproxOrLessThan(const ::yade::math::RealHP<N>& a, const ::yade::math::RealHP<N>& b, const ::yade::math::RealHP<N>& eps)
	{
		return a <= b || isEqualFuzzy(a, b, eps);
	}
} // end namespace internal

#define MAKRO_BRR(N) \
	namespace internal { \
	template <     > inline long double cast<typename ::yade::math::RealHP<N>, long double>(const ::yade::math::RealHP<N>& x) { return (long double)(x); } \
	template <     > inline double      cast<typename ::yade::math::RealHP<N>, double>(const ::yade::math::RealHP<N>& x) { return double(x); }\
	template <     > inline long        cast<typename ::yade::math::RealHP<N>, long>(const ::yade::math::RealHP<N>& x) { return long(x); }\
	template <     > inline int         cast<typename ::yade::math::RealHP<N>, int>(const ::yade::math::RealHP<N>& x) { return int(x); }\
	}

/*
	template <int N> inline long double cast<typename ::yade::math::RealHP<N>, long double>(const ::yade::math::RealHP<N>& x) { return (long double)(x); }
	template <int N> inline double      cast<typename ::yade::math::RealHP<N>, double>(const ::yade::math::RealHP<N>& x) { return double(x); }
	template <int N> inline long        cast<typename ::yade::math::RealHP<N>, long>(const ::yade::math::RealHP<N>& x) { return long(x); }
	template <int N> inline int         cast<typename ::yade::math::RealHP<N>, int>(const ::yade::math::RealHP<N>& x) { return int(x); }
*/

/* partial specialization not working:
template <typename Helper> using Hp = typename boost::enable_if_c<::yade::math::IsHP<Helper>,Helper>::type;
// boost::enable_if_c<::yade::math::IsHP<MaybeReal>,MaybeReal>::type
template <typename MaybeReal> struct NumTraits <Hp<MaybeReal>> : public NumTraitsHP<::yade::math::levelOfRealHP<MaybeReal>> {};
*/
/* partial specialization works only as a second template argument. So a modification in Eigen library is required for this to avoid macros:
template <typename L> struct NumTraits <L , typename std::enable_if<::yade::math::IsHP<L>>::type> : public NumTraitsHP<::yade::math::levelOfRealHP<L>> {};
*/
//template <int N> struct    NumTraitss<typename ::yade::math::detail::NthLevelRealHP<N>> : public NumTraitsHP<N> {}; // not working

#if (YADE_REAL_BIT >= 80)
template <> struct NumTraits<::yade::RealHP<1 >> : public NumTraitsHP<1 > {};
MAKRO_BRR(1 )
#endif

#if (YADE_REAL_BIT >= 64)
template <> struct NumTraits<::yade::RealHP<2 >> : public NumTraitsHP<2 > {};
MAKRO_BRR(2 )
#endif

#if (YADE_REAL_BIT >= 32)
template <> struct NumTraits<::yade::RealHP<3 >> : public NumTraitsHP<3 > {};
MAKRO_BRR(3 )
#endif

template <> struct NumTraits<::yade::RealHP<4 >> : public NumTraitsHP<4 > {};
MAKRO_BRR(4 )

template <> struct NumTraits<::yade::RealHP<5 >> : public NumTraitsHP<5 > {};
MAKRO_BRR(5 )

template <> struct NumTraits<::yade::RealHP<6 >> : public NumTraitsHP<6 > {};
MAKRO_BRR(6 )

template <> struct NumTraits<::yade::RealHP<7 >> : public NumTraitsHP<7 > {};
MAKRO_BRR(7 )

template <> struct NumTraits<::yade::RealHP<8 >> : public NumTraitsHP<8 > {};
MAKRO_BRR(8 )

template <> struct NumTraits<::yade::RealHP<9 >> : public NumTraitsHP<9 > {};
MAKRO_BRR(9 )

template <> struct NumTraits<::yade::RealHP<10>> : public NumTraitsHP<10> {};
MAKRO_BRR(10)
//MAKRO_BRR(11)
//MAKRO_BRR(12)

#undef MAKRO_BRR


/*************************************************************************/
/*************************       Complex        **************************/
/*************************************************************************/

// FIXME - use ComplexHP

template <> struct NumTraits<::yade::math::Complex> : GenericNumTraits<::yade::math::Complex> { // NOTE: Don't include this file for float, double, long double.
	typedef ::yade::math::Complex::value_type Real;
	typedef ::yade::math::Complex             Complex;
	typedef ::yade::math::Complex             NonInteger;
	typedef ::yade::math::Complex             Nested;

	enum { IsInteger             = 0,
	       IsSigned              = 1,
	       IsComplex             = 1,
	       RequireInitialization = 1,
	       ReadCost              = 2 * EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::ReadCost,
	       AddCost               = 2 * EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::AddCost,
	       MulCost               = 4 * EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::MulCost + 2 * EigenCostRealHP<::yade::math::UnderlyingRealHP<Real>>::AddCost };

	static constexpr long get_default_prec = std::numeric_limits<Real>::digits;

	static inline Complex highest(long = get_default_prec) { return std::numeric_limits<Real>::max(); }
	static inline Complex lowest(long = get_default_prec) { return std::numeric_limits<Real>::lowest(); }

	// Constants
	static inline Complex Pi(long = get_default_prec) { return boost::math::constants::pi<Real>(); }
	static inline Complex Euler(long = get_default_prec) { return boost::math::constants::euler<Real>(); }
	static inline Complex Log2(long = get_default_prec) { return ::yade::math::log(Real(2)); }
	static inline Complex Catalan(long = get_default_prec) { return boost::math::constants::catalan<Real>(); }

	static inline Complex epsilon(long = get_default_prec) { return std::numeric_limits<Real>::epsilon(); }
	static inline Complex epsilon(const Complex&) { return std::numeric_limits<Real>::epsilon(); }
	static inline Complex smallest_positive() { return std::numeric_limits<Real>::min(); }
	static inline int     digits10(long = get_default_prec) { return std::numeric_limits<Real>::digits10; }
	static inline int     digits10(const Complex&) { return std::numeric_limits<Real>::digits10; }
	static inline Complex dummy_precision() { return epsilon() * ::yade::math::pow(Real(10), digits10() / Real(10)); }
};

/*
namespace internal {
	template <> inline ::yade::math::Complex random<::yade::math::Complex>() { return ::yade::random(); }
	template <> inline ::yade::math::Complex random<::yade::math::Complex>(const ::yade::math::Complex& a, const ::yade::math::Complex& b)
	{
		return a + (b - a) * ::yade::random01();
	}
	inline bool isMuchSmallerThan(const ::yade::math::Complex& a, const ::yade::math::Complex& b, const ::yade::math::Complex& eps)
	{
		return ::yade::abs(a) <= ::yade::abs(b) * eps;
	}
	inline bool isEqualFuzzy(const ::yade::math::Complex& a, const ::yade::math::Complex& b, const ::yade::math::Complex& eps) { return ::yade::abs(a - b) <= eps; }
	inline bool isApprox(const ::yade::math::Complex& a, const ::yade::math::Complex& b, const ::yade::math::Complex& eps) { return isEqualFuzzy(a, b, eps); }
	inline bool isApproxOrLessThan(const ::yade::math::Complex& a, const ::yade::math::Complex& b, const ::yade::math::Complex& eps)
	{
		return a <= b || isEqualFuzzy(a, b, eps);
	}
	template <> inline long double cast<::yade::math::Complex, long double>(const ::yade::math::Complex& x) { return (long double)(x); }
	template <> inline double      cast<::yade::math::Complex, double>(const ::yade::math::Complex& x) { return double(x); }
	template <> inline long        cast<::yade::math::Complex, long>(const ::yade::math::Complex& x) { return long(x); }
	template <> inline int         cast<::yade::math::Complex, int>(const ::yade::math::Complex& x) { return int(x); }
} // end namespace internal
*/

}

/*
//namespace yade::Math { // maybe add this later

inline const ::yade::math::Real& conj(const ::yade::math::Real& x) { return x; }
inline const ::yade::math::Real& real(const ::yade::math::Real& x) { return x; }
inline ::yade::math::Real        imag(const ::yade::math::Real&) { return 0.; }
inline ::yade::math::Real        abs(const ::yade::math::Real& x)
{
	return ::yade::abs(x);
}
inline ::yade::math::Real abs2(const ::yade::math::Real& x) { return x * x; }

//}
*/

#endif
