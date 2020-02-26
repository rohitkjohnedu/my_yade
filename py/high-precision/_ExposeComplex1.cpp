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
void expose_complex1(){
	#ifdef _COMPLEX_SUPPORT
		py::class_<Vector2cr>("Vector2c","/*TODO*/",py::init<>()).def(VectorVisitor<Vector2cr>());
		py::class_<Vector3cr>("Vector3c","/*TODO*/",py::init<>()).def(VectorVisitor<Vector3cr>());
		py::class_<Vector6cr>("Vector6c","/*TODO*/",py::init<>()).def(VectorVisitor<Vector6cr>());
		py::class_<VectorXcr>("VectorXc","/*TODO*/",py::init<>()).def(VectorVisitor<VectorXcr>());
	#endif
}


