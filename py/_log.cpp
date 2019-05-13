// 2006-2008 © Václav Šmilauer
// 2019 Janek Kozicki
// hint: follow changes in d067b0696a8 to add new modules.

#include <core/Omega.hpp>
#include <lib/pyutil/doc_opts.hpp>

namespace py = boost::python;

py::list placeholder() {
	return {};
}

BOOST_PYTHON_MODULE(_log){
	YADE_SET_DOCSTRING_OPTS;
// We can use C++ string literal just like """ """ in python to write docstrings (see. https://en.cppreference.com/w/cpp/language/string_literal )
// The """ is a custom delimeter, we could use    R"RAW( instead, or any other delimeter. This decides what will be the termination delimeter.
	py::def("placeholder", placeholder, R"""(
.. warning:: I must write docstring here!
	)""");
}

/* this was in git revision 014b11496

#include<boost/python.hpp>
#include<string>
#include<lib/base/Logging.hpp>
#include<lib/pyutil/doc_opts.hpp>
using namespace boost;
enum{ll_TRACE,ll_DEBUG,ll_INFO,ll_WARN,ll_ERROR,ll_FATAL};

#ifdef YADE_LOG4CXX

	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.log");

	#include<log4cxx/logmanager.h>

	void logSetLevel(std::string loggerName,int level){
		std::string fullName(loggerName.empty()?"yade":("yade."+loggerName));
		if(!log4cxx::LogManager::exists(fullName)){
			LOG_WARN("No logger named "<<loggerName<<", ignoring level setting.");
			// throw std::invalid_argument("No logger named `"+fullName+"'");
		}
		log4cxx::LevelPtr l;
		switch(level){
			#ifdef LOG4CXX_TRACE
				case ll_TRACE: l=log4cxx::Level::getTrace(); break;
				case ll_DEBUG: l=log4cxx::Level::getDebug(); break;
				case ll_INFO:  l=log4cxx::Level::getInfo(); break;
				case ll_WARN:  l=log4cxx::Level::getWarn(); break;
				case ll_ERROR: l=log4cxx::Level::getError(); break;
				case ll_FATAL: l=log4cxx::Level::getFatal(); break;
			#else
				case ll_TRACE: l=log4cxx::Level::DEBUG; break;
				case ll_DEBUG: l=log4cxx::Level::DEBUG; break;
				case ll_INFO:  l=log4cxx::Level::INFO; break;
				case ll_WARN:  l=log4cxx::Level::WARN; break;
				case ll_ERROR: l=log4cxx::Level::ERROR; break;
				case ll_FATAL: l=log4cxx::Level::FATAL; break;
			#endif
			default: throw std::invalid_argument("Unrecognized logging level "+lexical_cast<std::string>(level));
		}
		log4cxx::LogManager::getLogger("yade."+loggerName)->setLevel(l);
	}
	void logLoadConfig(std::string f){ log4cxx::PropertyConfigurator::configure(f); }
#else
	bool warnedOnce=false;
	void logSetLevel(std::string loggerName, int level){
		// better somehow python's raise RuntimeWarning, but not sure how to do that from c++
		// it shouldn't be trapped by boost::python's exception translator, just print warning
		// Do like this for now.
		if(warnedOnce) return;
		LOG_WARN("Yade was compiled without boost::log support. Setting log levels from python will have no effect (warn once).");
		warnedOnce=true;
	}
	void logLoadConfig(std::string f){
		if(warnedOnce) return;
		LOG_WARN("Yade was compiled without boost::log support. Loading log file will have no effect (warn once).");
		warnedOnce=true;
	}
#endif

BOOST_PYTHON_MODULE(log){
	python::scope().attr("__doc__") = "Access and manipulation of log4cxx loggers.";

	YADE_SET_DOCSTRING_OPTS;

	python::def("setLevel",logSetLevel,(python::arg("logger"),python::arg("level")),"Set minimum severity *level* (constants ``TRACE``, ``DEBUG``, ``INFO``, ``WARN``, ``ERROR``, ``FATAL``) for given logger. \nLeading 'yade.' will be appended automatically to the logger name; if logger is '', the root logger 'yade' will be operated on.");
	python::def("loadConfig",logLoadConfig,(python::arg("fileName")),"Load configuration from file (log4cxx::PropertyConfigurator::configure)");
	python::scope().attr("TRACE")=(int)ll_TRACE;
	python::scope().attr("DEBUG")=(int)ll_DEBUG;
	python::scope().attr("INFO")= (int)ll_INFO;
	python::scope().attr("WARN")= (int)ll_WARN;
	python::scope().attr("ERROR")=(int)ll_ERROR;
	python::scope().attr("FATAL")=(int)ll_FATAL;
}

*/
