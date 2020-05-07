/*************************************************************************
*  2019 Janek Kozicki                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef REAL_TO_FROM_PYTHON_CONVERTER_HPP
#define REAL_TO_FROM_PYTHON_CONVERTER_HPP

#include <boost/python.hpp>

#include <lib/high-precision/RealIO.hpp>
#include <boost/lexical_cast.hpp>

namespace forCtags {
struct ToFromPythonConverter {
}; // for ctags
}

/*************************************************************************/
/*************************        Real          **************************/
/*************************************************************************/

// The note at the end of http://mpmath.org/doc/current/basics.html#temporarily-changing-the-precision
// indicates that having different mpmath variables with different precision is poorly supported.
// So python conversions of RealHP<N> for different precisions is questionable.
// Not a big problem, because N>=2 is supposed to be used only in critical C++ sections where better calculations are necessary.

template <typename ArbitraryReal> struct ArbitraryReal_to_python {
	static PyObject* convert(const ArbitraryReal& val)
	{
		// http://mpmath.org/doc/current/technical.html
		::boost::python::object mpmath = ::boost::python::import("mpmath");
		mpmath.attr("mp").attr("dps") = int(std::numeric_limits<ArbitraryReal>::digits10 + ::yade::math::extraDigits10NecessaryForStringRepresentation);
		::boost::python::object result = mpmath.attr("mpf")(::yade::math::toStringHP<ArbitraryReal>(val));
		return boost::python::incref(result.ptr());
	}
};

// https://www.boost.org/doc/libs/1_71_0/libs/python/doc/html/faq/how_can_i_automatically_convert_.html
template <typename ArbitraryReal> struct ArbitraryReal_from_python {
	ArbitraryReal_from_python() { boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<ArbitraryReal>()); }
	static void* convertible(PyObject* obj_ptr)
	{
		// using long strings or mpmath.mpf(…) object is the only way to get higher precision numbers into C++
		// The line below quickly accepts whatever python is able to convert into float, fortunately this also works for mpmath.mpf(…)
		// this can not work with val=0.123123123123123123123333312312333333123123123, the extra digits are cut-off by python before it reaches this function
		PyFloat_AsDouble(obj_ptr);
		// This quickly returns when argument wasn't a string.
		if (PyErr_Occurred() == nullptr)
			return obj_ptr;
		PyErr_Clear();
		// The quick way didn't work. There was an error, so let's clear it. And check if that is a string with a valid number inside.
		// This is a little more expensive. But it is used very rarely - only when user writes a python line like val="0.123123123123123123123333312312333333123123123"
		// otherwise only mpmath.mpf(NUMBER) objects are passed around inside python scripts which does not reach this line.
		std::istringstream ss { ::boost::python::call_method<std::string>(obj_ptr, "__str__") };
		ArbitraryReal      r;
		ss >> r;
		// Must reach end of string .eof(), otherwise it means there were illegal characters
		return ((not ss.fail()) and (ss.eof())) ? obj_ptr : nullptr;
	}
	static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		std::istringstream ss { ::boost::python::call_method<std::string>(obj_ptr, "__str__") };

		void* storage = ((boost::python::converter::rvalue_from_python_storage<ArbitraryReal>*)(data))->storage.bytes;
		new (storage) ArbitraryReal;
		ArbitraryReal* val = (ArbitraryReal*)storage;
		// ensure that "nan" "inf" are read correctly
		*val              = ::yade::math::fromStringRealHP<ArbitraryReal>(ss.str());
		data->convertible = storage;
	}
};

/*************************************************************************/
/*************************       Complex        **************************/
/*************************************************************************/

template <typename ArbitraryComplex> struct ArbitraryComplex_to_python {
	static PyObject* convert(const ArbitraryComplex& val)
	{
		std::stringstream ss_real {};
		std::stringstream ss_imag {};
		ss_real << ::yade::math::toStringHP<typename ArbitraryComplex::value_type>(val.real());
		ss_imag << ::yade::math::toStringHP<typename ArbitraryComplex::value_type>(val.imag());
		::boost::python::object mpmath = ::boost::python::import("mpmath");
		// http://mpmath.org/doc/current/technical.html
		mpmath.attr("mp").attr("dps") = int(
		        std::numeric_limits<typename ArbitraryComplex::value_type>::digits10 + ::yade::math::extraDigits10NecessaryForStringRepresentation);
		::boost::python::object result = mpmath.attr("mpc")(ss_real.str(), ss_imag.str());
		return boost::python::incref(result.ptr());
	}
};

// https://www.boost.org/doc/libs/1_71_0/libs/python/doc/html/faq/how_can_i_automatically_convert_.html
template <typename ArbitraryComplex> struct ArbitraryComplex_from_python {
	ArbitraryComplex_from_python() { boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<ArbitraryComplex>()); }
	static void* convertible(PyObject* obj_ptr)
	{
		// only python complex or mpmath.mpc(…) objects are supoprted. Strings are not parsed.
		// However a simple workaround is to write mpmath.mpc("1.211213123123123123123123123","-124234234.111")
		PyComplex_AsCComplex(obj_ptr);
		if (PyErr_Occurred() == nullptr)
			return obj_ptr;
		PyErr_Clear();
		return nullptr;
	}
	static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		std::istringstream ss_real { ::boost::python::call_method<std::string>(
			::boost::python::expect_non_null(PyObject_GetAttrString(obj_ptr, "real")), "__str__") };
		std::istringstream ss_imag { ::boost::python::call_method<std::string>(
			::boost::python::expect_non_null(PyObject_GetAttrString(obj_ptr, "imag")), "__str__") };
		void*              storage = ((boost::python::converter::rvalue_from_python_storage<ArbitraryComplex>*)(data))->storage.bytes;
		new (storage) ArbitraryComplex;
		ArbitraryComplex*                     val = (ArbitraryComplex*)storage;
		typename ArbitraryComplex::value_type re { 0 }, im { 0 };
		// ensure that "nan" "inf" are read correctly
		re                = ::yade::math::fromStringRealHP<typename ArbitraryComplex::value_type>(ss_real.str());
		im                = ::yade::math::fromStringRealHP<typename ArbitraryComplex::value_type>(ss_imag.str());
		*val              = ArbitraryComplex(re, im); // must explicitly call the constructor, static_cast won't work.
		data->convertible = storage;
	}
};

/*************************************************************************/
/************************* minieigenHP → string **************************/
/*************************************************************************/

// these are used by py/high-precision/minieigen/visitors.hpp
namespace yade {
namespace minieigenHP {
	inline std::string numToString(const int& num) { return ::boost::lexical_cast<::std::string>(num); } // ignore padding for now.

	// FIXME : template <typename Rr, int Level = levelOfRealHP<Rr>>
	inline std::string numToString(const ::yade::Real& num)
	{
		constexpr bool isPythonPrecisionEnough = std::is_same<double, ::yade::Real>::value or std::is_same<float, ::yade::Real>::value;
		if (isPythonPrecisionEnough) {
			return ::yade::math::toString(num);
		} else {
			// The only way to make sure that it is copy-pasteable without loss of precision is to put it inside "…"
			return "\"" + ::yade::math::toString(num) + "\"";
		}
	}

	inline std::string numToString(const ::yade::Complex& num)
	{
		constexpr bool isPythonPrecisionEnough
		        = std::is_same<double, ::yade::Complex::value_type>::value or std::is_same<float, ::yade::Complex::value_type>::value;
		std::string ret;
		if (num.real() != 0 && num.imag() != 0) {
			if (isPythonPrecisionEnough) {
				// don't add "+" in the middle if imag is negative and will start with "-"
				return numToString(num.real()) + (num.imag() > 0 ? "+" : "") + numToString(num.imag()) + "j";
			} else {
				// make sure it is copy-pasteable without loss of precision
				return "mpc(" + numToString(num.real()) + "," + numToString(num.imag()) + ")";
			}
		}
		// only imaginary is non-zero: skip the real part, and decrease padding to accomoadate the trailing "j"
		if (num.imag() != 0) {
			if (isPythonPrecisionEnough) {
				return numToString(num.imag()) + "j";
			} else {
				return "mpc(\"0\"," + numToString(num.imag()) + ")";
			}
		}
		if (isPythonPrecisionEnough) {
			return numToString(num.real());
		} else {
			return "mpc(" + numToString(num.real()) + ",\"0\")";
		}
	}

}
}

#endif
