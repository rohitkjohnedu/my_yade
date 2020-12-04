#include <lib/base/Logging.hpp>
#include <lib/pyutil/gil.hpp>

CREATE_CPP_LOCAL_LOGGER("gil.cpp")
void pyRunString(const std::string& cmd)
{
	// FIXME: when a new function is declared inside ipython session an extra python command must be called: globals().update(locals())
	//        https://stackoverflow.com/questions/43956636/internal-function-call-not-working-in-ipython
	//        calling it always here, before each call to cmd.c_str() is worth discussing.
	//        For now I will just add a note about that in documentation: doc/sphinx/tutorial-data-mining.rst
	gilLock lock;
	auto    result = PyRun_SimpleString(cmd.c_str());
	if (result != 0) {
		LOG_FATAL("PyRunner → pyRunString encountered exception while executing command: '" << cmd << "'");
		// An exception occurred. Unfortunately the memory barrier between both threads makes it rather difficult to find out what kind of exception it was.
		// So let's just print it and throw a runtime_error.
		PyErr_Print();
		PyErr_SetString(PyExc_SystemError, __FILE__);
		// FIXME : find a way to extract exception from python and rethrow the same exception. It will have to be a "reconstruction" (not even a copy)
		//         of the original exception though, because here we are inside the memory region of a separate thread.
		throw std::runtime_error("PyRunner → pyRunString encountered exception while executing command: '" + cmd + "'");
	}
};
