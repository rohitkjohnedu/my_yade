/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef _HIGH_PRECISION_SUPPORT
// compilation wall clock time: 0:19.73 → split into two files → 0:12.70
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
using namespace ::yade::MathEigenTypes;

#ifndef EIGEN_DONT_ALIGN
// this is very funny. When compiling vecorized code the Vector3r is based on AlignedVector3. And then the line 'py::class_<Vector3ra>(…)' is trying to register
// AlignedVector3 for the second time, because the Vector3r==AlignedVector3 is already registered. Which results in a runtime error.
// So this define is only to avoid duplicate registration.
#define EIGEN_DONT_ALIGN
#define UNDEF_EIGEN_DONT_ALIGN
#endif

// half of minieigen/expose-vectors.cpp
#include <py/high-precision/minieigen/visitors.hpp>
void expose_vectors1(){
	py::class_<VectorXr>("VectorX","Dynamic-sized float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a VectorX): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of X floats.",py::init<>())
		.def(VectorVisitor<VectorXr>())
	;


	py::class_<Vector6r>("Vector6","6-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector6): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of 6 floats.\n\nStatic attributes: ``Zero``, ``Ones``.",py::init<>())
		.def(VectorVisitor<Vector6r>())
	;

	py::class_<Vector6i>("Vector6i","6-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector6): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of 6 floats.\n\nStatic attributes: ``Zero``, ``Ones``.",py::init<>())
		.def(VectorVisitor<Vector6i>())
	;

		py::class_<Vector4r>("Vector4","4-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of 4 floats.\n\nStatic attributes: ``Zero``, ``Ones``.",py::init<>())
			.def(VectorVisitor<Vector4r>())
		;

}

#ifdef UNDEF_EIGEN_DONT_ALIGN
#undef EIGEN_DONT_ALIGN
#undef UNDEF_EIGEN_DONT_ALIGN
#endif

#endif

