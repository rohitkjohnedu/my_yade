/*************************************************************************
*  2009-2012 © Václav Šmilauer                                           *
*  2019        Janek Kozicki                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// When yade uses high-precision number as Real type the usual (old) 'import minieigen'
// has to be replaced with a minieigenHP library which uses exacly the same number of decimal places
// as yade is using everywhere else. Please note that such precision can be very arbitrary, because cpp_bin_float
// or mpfr take it as a compile-time argument. Hence such minieigenHP cannot be separately precompiled as a package.
// Though it could be precompiled for some special types such as boost::multiprecision::float128

#include <lib/base/Logging.hpp>
#include <lib/high-precision/Real.hpp>
#include <lib/high-precision/RealHPInfo.hpp>
#include <lib/high-precision/ToFromPythonConverter.hpp>
#include <lib/pyutil/doc_opts.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <py/high-precision/_ExposeStorageOrdering.hpp>
#include <sstream>

using namespace ::yade::MathEigenTypes;
#include <py/high-precision/minieigen/converters.hpp>
#include <py/high-precision/minieigen/expose.hpp>
#include <py/high-precision/minieigen/visitors.hpp>

CREATE_CPP_LOCAL_LOGGER("_minieigenHP.cpp")

template <int N, bool registerConverters> struct RegisterEigenHP {
	static void work(const py::scope& topScope, const py::scope& scopeHP)
	{
		// FIXME this section and remove duplicate registrations.
		// https://gitlab.com/cosurgi/minieigen-real specific stuff: START
		//		if (registerConverters) {
		py::scope top(topScope);
		ArbitraryComplex_from_python<ComplexHP<N>>();
		py::to_python_converter<ComplexHP<N>, ArbitraryComplex_to_python<ComplexHP<N>>>();

		ArbitraryReal_from_python<RealHP<N>>();
		py::to_python_converter<RealHP<N>, ArbitraryReal_to_python<RealHP<N>>>();
		//		}
		// https://gitlab.com/cosurgi/minieigen-real specific stuff: END

		py::scope HPn(scopeHP);

		expose_converters<N>(); // in _ExposeConverters.cpp

#ifdef EIGEN_DONT_ALIGN
		py::scope().attr("vectorize") = false;
#else
		py::scope().attr("vectorize") = true;
		// when we use vectorization the Vector3r is AlignedVector3, so we need to register converter from plain old Vector3<Real> so that other functions can accept it as an argument
		custom_VectorAnyAny_from_sequence<Eigen::Matrix<RealHP<N>, 3, 1>>();
		py::class_<Eigen::Matrix<RealHP<N>, 3, 1>>(
		        "Vector3na",
		        "3-dimensional non-aligned float vector; same as :obj:`Vector3`, but with alignment (``Eigen::AlignedVector3``).\n\nSupported "
		        "operations "
		        "(``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, "
		        "``v/=f``, "
		        "``v==v``, ``v!=v``, plus operations with ``Matrix3`` and ``Quaternion``.\n\nImplicit conversion from sequence (list, tuple, ...) of 3 "
		        "floats.\n\nStatic attributes: ``Zero``, ``Ones``, ``UnitX``, ``UnitY``, ``UnitZ``.",
		        py::init<>())
		        .def(VectorVisitor<Eigen::Matrix<RealHP<N>, 3, 1>>());
#endif

		expose_vectors1<N>();
		expose_vectors2<N>();
		expose_matrices1<N>(); // must come after vectors
		expose_matrices2<N>(); // must come after vectors
		expose_complex1<N>();
		expose_complex2<N>();
		expose_quaternion<N>();
		expose_boxes<N>();
	}
};

BOOST_PYTHON_MODULE(THE_CPP_NAME)
try {
	YADE_SET_DOCSTRING_OPTS;

	// https://gitlab.com/cosurgi/minieigen-real specific stuff: START
	expose_storage_ordering();
	// https://gitlab.com/cosurgi/minieigen-real specific stuff: END

	py::scope().attr("__doc__") = "miniEigen is wrapper for a small part of the `Eigen <http://eigen.tuxfamily.org>`_ library. Refer to its documentation "
	                              "for details. All classes in this module support pickling.";

	const constexpr int highestPythonRegisteredHP_N = 10;
	::yade::math::detail::registerLoopForHPn<highestPythonRegisteredHP_N, RegisterEigenHP>();

	// FIXME - add struct BasicInfoAboutHP ?? with extraDigits10NecessaryForStringRepresentation, highestPythonRegisteredHP_N, digits10RealHP inside?
	py::scope().attr("extraStringDigits") = ::yade::math::extraDigits10NecessaryForStringRepresentation;
	py::scope().attr("maxRealLevelHP")    = highestPythonRegisteredHP_N;
	py::def("getRealHPSupportedByEigenCgal", ::yade::math::RealHPInfo::getRealHPSupportedByEigenCgal);
	py::def("getRealHPSupportedByMinieigen", ::yade::math::RealHPInfo::getRealHPSupportedByMinieigen);
	py::def("getRealHPDigits10", ::yade::math::RealHPInfo::getRealHPDigits10, (py::arg("N")));

} catch (...) {
	LOG_FATAL("Importing this module caused an exception and this module is in an inconsistent state now.");
	PyErr_Print();
	PyErr_SetString(PyExc_SystemError, __FILE__);
	boost::python::handle_exception();
	throw;
}
