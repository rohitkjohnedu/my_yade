#include <py/_libVersions.hpp>

namespace py = boost::python;

// Each of those functions will return following LibraryVersionInfo:
// 1. string libraryName
// 2. string libraryVersion as provided by library authors
// 3. tuple<int,int,int> a tuple of version numbers (major,minor,patch)
// 4. float libraryVersionFloat cunstructed from major.minor version number.

BOOST_PYTHON_MODULE(_libVersions){
	YADE_SET_DOCSTRING_OPTS;
	py::def("mpiVer", mpiVer, "Returns MPI library version with which yade was compiled, as detected from ``#include <mpi.h>``.");
	py::def("vtkVer", vtkVer, "VTK ver");
	py::def("cgalVer", cgalVer, "CGAL ver");
}

