#pragma once
#include<boost/version.hpp>

// macro to set the same docstring generation options in all modules
// disable_cpp_signatures apparently appeared after 1.35 or 1.34
#if BOOST_VERSION<103600
	#define YADE_SET_DOCSTRING_OPTS python::docstring_options docopt; docopt.enable_all();
#else
	#define YADE_SET_DOCSTRING_OPTS python::docstring_options docopt; docopt.enable_all(); docopt.disable_cpp_signatures();
#endif

