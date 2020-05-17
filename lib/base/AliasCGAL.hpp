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

template<>
class ERealHP<1> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<1>>::Base<ERealHP<1>>::Type, ERealHP<1>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<2> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<2>>::Base<ERealHP<2>>::Type, ERealHP<2>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<3> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<3>>::Base<ERealHP<3>>::Type, ERealHP<3>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<4> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<4>>::Base<ERealHP<4>>::Type, ERealHP<4>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<5> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<5>>::Base<ERealHP<5>>::Type, ERealHP<5>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<6> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<6>>::Base<ERealHP<6>>::Type, ERealHP<6>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<7> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<7>>::Base<ERealHP<7>>::Type, ERealHP<7>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<8> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<8>>::Base<ERealHP<8>>::Type, ERealHP<8>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<9> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<9>>::Base<ERealHP<9>>::Type, ERealHP<9>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};

template<>
class ERealHP<10> : public Filtered_kernel_adaptor<
                      Type_equality_wrapper<Simple_cartesian<::yade::RealHP<10>>::Base<ERealHP<10>>::Type, ERealHP<10>>,
#ifdef CGAL_NO_STATIC_FILTERS
                      false>
#else
                      true>
#endif
{
};


using Exact_Real_predicates_inexact_constructions_kernel = ERealHP<1>;
template <int N> using Exact_RealHP_predicates_inexact_constructions_kernel = ERealHP<N>;

template <> struct Triangulation_structural_filtering_traits<ERealHP<1>> { // FIXME ←←← tu jest wymuszone 1
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<2>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<3>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<4>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<5>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<6>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<7>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<8>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<9>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

template <> struct Triangulation_structural_filtering_traits<ERealHP<10>> { // FIXME ←←← tu jest wymuszone 2
	typedef Tag_true Use_structural_filtering_tag;
};

} // namespace CGAL

namespace yade {

// CGAL definitions - does not work with another kernel!! Why???
//   / Answer: because to use exact kernal yade must link with mpfr / Janek.
//   / to do this have a look at CMakeLists 'Real precision' section

// These are taken from files pkg/dem/Polyhedra.hpp and /pkg/dem/Gl1_PotentialBlock.hpp
// let's call this CGP for CGal Polyhedrons namespace. It uses inexact kernel. Later when mpfr support is added we can have similar
// aliases in other namespace for exact kernel. Or maybe later rename namespace into struct, so that this can be used as template argument.
// We will see how this will need to evelove.
//	namespace CGP {

// FIXME : template <int N> struct CgalHP { using ……… };

using K              = CGAL::Exact_Real_predicates_inexact_constructions_kernel;
template <int N> using K_HP              = CGAL::Exact_RealHP_predicates_inexact_constructions_kernel<N>; // FIXME - this line and all the others.
using Polyhedron     = CGAL::Polyhedron_3<K>;
using Mesh           = CGAL::Surface_mesh<K::Point_3>;
using Triangulation  = CGAL::Delaunay_triangulation_3<K>;
using CGALpoint      = K::Point_3;
template <int N> using CGALpoint_HP      = typename K_HP<N>::Point_3;
using CGALtriangle   = K::Triangle_3;
using CGALvector     = K::Vector_3;
template<int N> using CGALvector_HP     = typename K_HP<N>::Vector_3;
using Transformation = CGAL::Aff_transformation_3<K>;
using Segment        = K::Segment_3;
using Triangle       = CGAL::Triangle_3<K>;
using Plane          = CGAL::Plane_3<K>;
template <int N> using Plane_HP         = typename CGAL::Plane_3<K_HP<N>>;
using Line           = CGAL::Line_3<K>;
using CGAL_ORIGIN    = CGAL::Origin;
using CGAL_AABB_tree = CGAL::AABB_tree<CGAL::AABB_traits<K, CGAL::AABB_triangle_primitive<K, std::vector<Triangle>::iterator>>>;
//	}
// It would be perhaps useful to collect here similar stuff frmo other files.
} // namespace yade

#endif
