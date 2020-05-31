/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// compilation wall clock time: 0:24.22 → split into two files → 0:13.11
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
using namespace ::yade::MathEigenTypes;
// define this for compatibility with minieigen.
#define _COMPLEX_SUPPORT
// half of minieigen/expose-complex.cpp file
#include <py/high-precision/minieigen/visitors.hpp>
template <int N> void expose_complex1()
{
#ifdef _COMPLEX_SUPPORT
	py::class_<Vector2crHP<N>>("Vector2c", "/*TODO*/", py::init<>()).def(VectorVisitor<Vector2crHP<N>>());
	py::class_<Vector3crHP<N>>("Vector3c", "/*TODO*/", py::init<>()).def(VectorVisitor<Vector3crHP<N>>());
	py::class_<Vector6crHP<N>>("Vector6c", "/*TODO*/", py::init<>()).def(VectorVisitor<Vector6crHP<N>>());
	py::class_<VectorXcrHP<N>>("VectorXc", "/*TODO*/", py::init<>()).def(VectorVisitor<VectorXcrHP<N>>());
#endif
}

// explicit instantination - tell compiler to produce a compiled version of expose_converters (it is faster when done in parallel in .cpp files)
YADE_EIGEN_HP_EXPLICIT_INSTATINATION_OF_PYTHON_CONVERTER(expose_complex1)

