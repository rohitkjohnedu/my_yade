/*************************************************************************
*  2012-2020 Václav Šmilauer                                             *
*  2020      Janek Kozicki                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// compilation wall clock time: 0:05.80
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
using namespace ::yade::MathEigenTypes;
// define this for compatibility with minieigen.
#define _COMPLEX_SUPPORT
// file minieigen/expose-converters.cpp
#include <py/high-precision/minieigen/converters.hpp>
void expose_converters()
{
	custom_VectorAnyAny_from_sequence<VectorXr>();
	custom_VectorAnyAny_from_sequence<Vector6r>();
	custom_VectorAnyAny_from_sequence<Vector6i>();
	custom_VectorAnyAny_from_sequence<Vector3r>();
	custom_VectorAnyAny_from_sequence<Vector3i>();
	custom_VectorAnyAny_from_sequence<Vector2r>();
	custom_VectorAnyAny_from_sequence<Vector2i>();
	custom_alignedBoxNr_from_seq<2>();
	custom_alignedBoxNr_from_seq<3>();
	custom_Quaternionr_from_axisAngle_or_angleAxis();

	custom_MatrixAnyAny_from_sequence<Matrix3r>();
	custom_MatrixAnyAny_from_sequence<Matrix6r>();
	custom_MatrixAnyAny_from_sequence<MatrixXr>();

#ifdef _COMPLEX_SUPPORT
	custom_VectorAnyAny_from_sequence<Vector2cr>();
	custom_VectorAnyAny_from_sequence<Vector3cr>();
	custom_VectorAnyAny_from_sequence<Vector6cr>();
	custom_VectorAnyAny_from_sequence<VectorXcr>();
	custom_MatrixAnyAny_from_sequence<Matrix3cr>();
	custom_MatrixAnyAny_from_sequence<Matrix6cr>();
	custom_MatrixAnyAny_from_sequence<MatrixXcr>();
#endif
}
