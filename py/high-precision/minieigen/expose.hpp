/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// functions defined in the respective .cpp files
// N is the level of RealHP<N>
template <int N> void expose_matrices1();
template <int N> void expose_matrices2();
template <int N> void expose_vectors1();
template <int N> void expose_vectors2();
template <int N> void expose_boxes();
template <int N> void expose_quaternion();
template <int N> void expose_complex1(); // does nothing if _COMPLEX_SUPPORT is not #defined
template <int N> void expose_complex2(); // does nothing if _COMPLEX_SUPPORT is not #defined
template <int N> void expose_converters();
