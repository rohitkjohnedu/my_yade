/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// compilation wall clock time: 0:19.73 → split into two files → 0:11.67
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
void expose_vectors2()
{
	py::class_<Vector3r>(
	        "Vector3",
	        "3-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, "
	        "``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``, plus operations with ``Matrix3`` and ``Quaternion``.\n\nImplicit "
	        "conversion from sequence (list, tuple, ...) of 3 floats.\n\nStatic attributes: ``Zero``, ``Ones``, ``UnitX``, ``UnitY``, ``UnitZ``.",
	        py::init<>())
	        .def(VectorVisitor<Vector3r>());
#ifndef EIGEN_DONT_ALIGN
	py::class_<Vector3ra>(
	        "Vector3a",
	        "3-dimensional float vector; same as :obj:`Vector3`, but with alignment (``Eigen::AlignedVector3``).\n\nSupported operations (``f`` if a "
	        "float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, "
	        "``v!=v``, plus operations with ``Matrix3`` and ``Quaternion``.\n\nImplicit conversion from sequence (list, tuple, ...) of 3 floats.\n\nStatic "
	        "attributes: ``Zero``, ``Ones``, ``UnitX``, ``UnitY``, ``UnitZ``.",
	        py::init<>())
	        .def(VectorVisitor<Vector3ra>());
#endif

	py::class_<Vector3i>(
	        "Vector3i",
	        "3-dimensional integer vector.\n\nSupported operations (``i`` if an int, ``v`` is a Vector3i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, "
	        "``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence  (list, tuple, ...) of 3 integers.\n\nStatic attributes: "
	        "``Zero``, ``Ones``, ``UnitX``, ``UnitY``, ``UnitZ``.",
	        py::init<>())
	        .def(VectorVisitor<Vector3i>());

	py::class_<Vector2r>(
	        "Vector2",
	        "3-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, "
	        "``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of 2 "
	        "floats.\n\nStatic attributes: ``Zero``, ``Ones``, ``UnitX``, ``UnitY``.",
	        py::init<>())
	        .def(VectorVisitor<Vector2r>());
	py::class_<Vector2i>(
	        "Vector2i",
	        "2-dimensional integer vector.\n\nSupported operations (``i`` if an int, ``v`` is a Vector2i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, "
	        "``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list, tuple, ...) of 2 integers.\n\nStatic attributes: "
	        "``Zero``, ``Ones``, ``UnitX``, ``UnitY``.",
	        py::init<>())
	        .def(VectorVisitor<Vector2i>());
}

#ifdef UNDEF_EIGEN_DONT_ALIGN
#undef EIGEN_DONT_ALIGN
#undef UNDEF_EIGEN_DONT_ALIGN
#endif
