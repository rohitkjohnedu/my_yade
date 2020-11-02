/*************************************************************************
*  2020 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <lib/high-precision/Constants.hpp>

namespace yade {
namespace math {

// TODO: replace with numeric_limits.
template <> const Real Math<Real>::EPSILON        = DBL_EPSILON;
template <> const Real Math<Real>::ZERO_TOLERANCE = 1e-20;
template <> const Real Math<Real>::MAX_REAL       = DBL_MAX;

template <> const Real Math<Real>::PI         = boost::math::constants::pi<Real>();
template <> const Real Math<Real>::TWO_PI     = boost::math::constants::two_pi<Real>();
template <> const Real Math<Real>::HALF_PI    = boost::math::constants::half_pi<Real>();
template <> const Real Math<Real>::DEG_TO_RAD = Math<Real>::PI / Real(180);
template <> const Real Math<Real>::RAD_TO_DEG = Real(180) / Math<Real>::PI;

} // namespace math
} // namespace yade

