#ifdef _HIGH_PRECISION_SUPPORT
// compilation wall clock time: 0:06.92
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
using namespace ::yade::MathEigenTypes;
// file minieigen/expose-boxes.cpp
#include <py/high-precision/minieigen/visitors.hpp>
void expose_boxes(){
	py::class_<AlignedBox3r>("AlignedBox3","Axis-aligned box object, defined by its minimum and maximum corners",py::init<>())
		.def(AabbVisitor<AlignedBox3r>())
	;

	py::class_<AlignedBox2r>("AlignedBox2","Axis-aligned box object in 2d, defined by its minimum and maximum corners",py::init<>())
		.def(AabbVisitor<AlignedBox2r>())
	;
}


#endif

