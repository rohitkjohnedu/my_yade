#ifdef _HIGH_PRECISION_SUPPORT
// wall clock time: 0:19.73 → split into two files
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

#include <minieigen/expose-vectors.cpp>

#ifdef UNDEF_EIGEN_DONT_ALIGN
#undef EIGEN_DONT_ALIGN
#undef UNDEF_EIGEN_DONT_ALIGN
#endif

#endif

