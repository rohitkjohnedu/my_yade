/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// compilation wall clock time: 0:24.22 → split into two files → 0:16.20
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
using namespace ::yade::MathEigenTypes;
// define this for compatibility with minieigen.
#define _COMPLEX_SUPPORT
// half of minieigen/expose-complex.cpp file
#include <py/high-precision/minieigen/visitors.hpp>
void expose_complex2()
{
#ifdef _COMPLEX_SUPPORT
	py::class_<Matrix3cr>("Matrix3c", "/*TODO*/", py::init<>()).def(MatrixVisitor<Matrix3cr>());
	py::class_<Matrix6cr>("Matrix6c", "/*TODO*/", py::init<>()).def(MatrixVisitor<Matrix6cr>());
	py::class_<MatrixXcr>("MatrixXc", "/*TODO*/", py::init<>()).def(MatrixVisitor<MatrixXcr>());
#endif
}
