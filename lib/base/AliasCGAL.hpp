/*************************************************************************
*  2019 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

// This file is to collect all CGAL aliases inside yade sub-namespace

#ifdef YADE_CGAL

#include <lib/high-precision/Real.hpp>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Tetrahedron_3.h>
#include <CGAL/Triangulation_data_structure_3.h>
#include <CGAL/Triangulation_structural_filtering_traits.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/squared_distance_3.h>
/*
namespace CGAL {
// The Exact_predicates_inexact_constructions_kernel used `double`. Make corresponding typedef for Real type.
//class EReal : public Filtered_kernel_adaptor<
//                      Type_equality_wrapper<Simple_cartesian<::yade::Real>::Base<EReal>::Type, EReal>,
template <typename Rr , int N = ::yade::math::levelOfRealHP<Rr>>
class ERealHP : public Filtered_kernel_adaptor<
//                    Type_equality_wrapper<typename Simple_cartesian< typename ::yade::RealHP<N> >::Base<typename ERealHP<N > >::Type , (typename ERealHP<N >) >,
                      Type_equality_wrapper<         Simple_cartesian<                  Rr        >::Base<         ERealHP<Rr> >::Type ,           ERealHP<Rr>  >,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};
*/

namespace CGAL {
// The Exact_predicates_inexact_constructions_kernel used `double`. Make corresponding typedef for Real type.
template <int L> class ERealHP;

// There are two ways to avoid this macro (hint: the best is to use C++20). See file lib/high-precision/ExplicitRealHP.hpp for details.
#ifdef CGAL_NO_STATIC_FILTERS
#define YADE_REAL_HP_CGAL_KERNEL_ADAPTOR(levelHP)                                                                                                              \
	template <>                                                                                                                                            \
	class ERealHP<levelHP> : public Filtered_kernel_adaptor<                                                                                               \
	                                 Type_equality_wrapper<Simple_cartesian<::yade::RealHP<levelHP>>::Base<ERealHP<levelHP>>::Type, ERealHP<levelHP>>,     \
	                                 false> {                                                                                                              \
	};
#else
#define YADE_REAL_HP_CGAL_KERNEL_ADAPTOR(levelHP)                                                                                                              \
	template <>                                                                                                                                            \
	class ERealHP<levelHP> : public Filtered_kernel_adaptor<                                                                                               \
	                                 Type_equality_wrapper<Simple_cartesian<::yade::RealHP<levelHP>>::Base<ERealHP<levelHP>>::Type, ERealHP<levelHP>>,     \
	                                 true> {                                                                                                               \
	};
#endif

// There are two ways to avoid this macro (hint: the best is to use C++20). See file lib/high-precision/ExplicitRealHP.hpp for details.
YADE_HP_RUN_MACRO(YADE_REAL_HP_CGAL_KERNEL_ADAPTOR)
#undef YADE_REAL_HP_CGAL_KERNEL_ADAPTOR

using Exact_Real_predicates_inexact_constructions_kernel                          = ERealHP<1>;
template <int levelHP> using Exact_RealHP_predicates_inexact_constructions_kernel = ERealHP<levelHP>;

#define YADE_REAL_HP_TRIANGULATION(levelHP)                                                                                                                    \
	template <> struct Triangulation_structural_filtering_traits<ERealHP<levelHP>> {                                                                       \
		typedef Tag_true Use_structural_filtering_tag;                                                                                                 \
	};

// There are two ways to avoid this macro (hint: the best is to use C++20). See file lib/high-precision/ExplicitRealHP.hpp for details.
YADE_HP_RUN_MACRO(YADE_REAL_HP_TRIANGULATION)
#undef YADE_REAL_HP_TRIANGULATION

} // namespace CGAL

namespace yade {

// CGAL definitions - does not work with another kernel!! Why???
//   / Answer: because to use exact kernel yade must link with mpfr / Janek.
//   / to do this have a look at CMakeLists 'Real precision' section

// These are taken from files pkg/dem/Polyhedra.hpp and /pkg/dem/Gl1_PotentialBlock.hpp
// let's call this CGP for CGal Polyhedrons namespace. It uses inexact kernel. Later when mpfr support is added we can have similar
// aliases in other namespace for exact kernel. Or maybe later rename namespace into struct, so that this can be used as template argument.
// We will see how this will need to evelove.
//	namespace CGP {

// FIXME : template <int N> struct CgalHP { using ……… };
// FIXME : use better names

using K                              = CGAL::Exact_Real_predicates_inexact_constructions_kernel;
template <int N> using K_HP          = CGAL::Exact_RealHP_predicates_inexact_constructions_kernel<N>; // FIXME - this line and all the others.
using Polyhedron                     = CGAL::Polyhedron_3<K>;
using Mesh                           = CGAL::Surface_mesh<K::Point_3>;
using Triangulation                  = CGAL::Delaunay_triangulation_3<K>;
using CGALpoint                      = K::Point_3;
template <int N> using CGALpoint_HP  = typename K_HP<N>::Point_3;
using CGALtriangle                   = K::Triangle_3;
using CGALvector                     = K::Vector_3;
template <int N> using CGALvector_HP = typename K_HP<N>::Vector_3;
using Transformation                 = CGAL::Aff_transformation_3<K>;
using Segment                        = K::Segment_3;
using Triangle                       = CGAL::Triangle_3<K>;
using Plane                          = CGAL::Plane_3<K>;
template <int N> using Plane_HP      = typename CGAL::Plane_3<K_HP<N>>;
using Line                           = CGAL::Line_3<K>;
using CGAL_ORIGIN                    = CGAL::Origin;
using CGAL_AABB_tree                 = CGAL::AABB_tree<CGAL::AABB_traits<K, CGAL::AABB_triangle_primitive<K, std::vector<Triangle>::iterator>>>;
//	}
// It would be perhaps useful to collect here similar stuff from other files.
} // namespace yade

#endif
