#include <core/Omega.hpp>
#include <lib/pyutil/doc_opts.hpp>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/python/tuple.hpp>

namespace py = boost::python;

#ifdef YADE_MPI
        #include <mpi.h>
// https://www.open-mpi.org/software/ompi/versions/
        py::list mpiVer() {
		py::list ret;
		ret.append( py::make_tuple                  (OMPI_MAJOR_VERSION   ,                                   OMPI_MINOR_VERSION  ,                                    OMPI_RELEASE_VERSION ));
// I didn't find a way to obtain a version string for mpi, so I construct my own. But if there is some MPI version string, better put it here.
//		ret.append( boost::lexical_cast<std::string>(OMPI_VERSION) );
		ret.append( boost::lexical_cast<std::string>(OMPI_MAJOR_VERSION)+"."+boost::lexical_cast<std::string>(OMPI_MINOR_VERSION)+"."+boost::lexical_cast<std::string>(OMPI_RELEASE_VERSION) );
		return ret;
	}
#else
        py::list mpiVer() { return {}; }
#endif

#ifdef YADE_VTK
        #include <vtkVersion.h>
        py::list vtkVer() {
		py::list ret;
		ret.append( py::make_tuple (VTK_MAJOR_VERSION , VTK_MINOR_VERSION , VTK_BUILD_VERSION ));
		ret.append( boost::lexical_cast<std::string>(VTK_VERSION) );
		return ret;
	}
#else
        py::list mpiVer() { return {}; }
#endif

#ifdef YADE_CGAL
        #include <CGAL/version_macros.h>
        py::list cgalVer() {
		py::list ret;
		ret.append( py::make_tuple( CGAL_VERSION_MAJOR , CGAL_VERSION_MINOR , CGAL_VERSION_PATCH ) );
		ret.append( boost::lexical_cast<std::string>(CGAL_VERSION) );
		return ret;
	}
#else
        py::list cgalVer() { return {}; }
#endif

py::dict allVersionsCpp(){
	py::dict ret;
	ret["mpi"   ] = mpiVer();
	ret["vtk"   ] = vtkVer();
	ret["cgal"  ] = cgalVer();
	return ret;
}

BOOST_PYTHON_MODULE(_libVersions){
	YADE_SET_DOCSTRING_OPTS;
	py::def("mpiVer"  , mpiVer , "Returns MPI   library version with which yade was compiled, as detected from ``#include                 <mpi.h>``.\n\n:return: a list in format ``[ (major,minor,patch) , \"versionString\" ]``.");
	py::def("vtkVer"  , vtkVer , "Returns VTK   library version with which yade was compiled, as detected from ``#include          <vtkVersion.h>``.\n\n:return: a list in format ``[ (major,minor,patch) , \"versionString\" ]``.");
	py::def("cgalVer" , cgalVer, "Returns CGAL  library version with which yade was compiled, as detected from ``#include <CGAL/version_macros.h>``.\n\n:return: a list in format ``[ (major,minor,patch) , \"versionString\" ]``.");

	py::def("allVersionsCpp", allVersionsCpp, R"""(
This function returns library versions as discovered by C++ during compilation from all the ``#include`` headers. This can be useful in debugging to detect some library ``.so`` conflicts.

:return: dictionary in folowing format: ``{ "libName" : [ (major,minor,patch) , "versionString" ] }``

As an example the table below reflects with what libraries this documentation was compiled (here are only those detected by C++):

.. ipython::

	   In [1]: from yade._libVersions import *

	   In [1]: allVersionsCpp()

.. note:: Please add here C++ detection of more libraries as yade starts using them.

	)""");
}

