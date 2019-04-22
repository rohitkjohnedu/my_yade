#pragma once
#include <core/Omega.hpp>
#include <lib/pyutil/doc_opts.hpp>
#include <iostream>

#ifdef YADE_MPI
	#include <mpi.h>
	void mpiVer() { std::cout << "openmpi: " <<  OMPI_MAJOR_VERSION << " " << OMPI_MINOR_VERSION << " " << OMPI_RELEASE_VERSION << "\n"; }
#else
	void mpiVer() { std::cout << "NO MPI ver\n"; return; }
#endif

#ifdef YADE_VTK
	#include <vtkVersion.h>
	void vtkVer() { std::cout << "vtk: " <<  VTK_MAJOR_VERSION << " " << VTK_MINOR_VERSION << " " << VTK_BUILD_VERSION << "\n"; }
#else
	void mpiVer() { std::cout << "NO VTK ver\n"; return; }
#endif

#ifdef YADE_CGAL
	#include <CGAL/version_macros.h>
	void cgalVer() { std::cout << "CGAL: " << CGAL_VERSION_MAJOR << " " << CGAL_VERSION_MINOR << " " << CGAL_VERSION_PATCH << "\n"; }
#else
	void cgalVer() { std::cout << "NO CGAL ver\n"; return; }
#endif

