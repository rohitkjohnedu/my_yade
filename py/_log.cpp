// 2006-2008 © Václav Šmilauer
// 2019 Janek Kozicki
// hint: follow changes in d067b0696a8 to add new modules.

#include<core/Omega.hpp>
#include<lib/pyutil/doc_opts.hpp>
#include<lib/base/Logging.hpp>
#include<string>

CREATE_CPP_LOCAL_LOGGER("_log.cpp");

namespace py = boost::python;

/*
py::list placeholder() {
	return {};
}
*/

void testAllLevels() {
	int testInt     = 0;
	std::string testStr = "test_string";
	Real testReal(11);
	Vector3r testVec(1,2,3);
	Matrix3r testMat = (Matrix3r() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished();
	std::complex<Real> testComplex(-1,1);

	LOG_6_TRACE   ("Test log level: LOG_6_TRACE   , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_5_DEBUG   ("Test log level: LOG_5_DEBUG   , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_4_INFO    ("Test log level: LOG_4_INFO    , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_3_WARN    ("Test log level: LOG_3_WARN    , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_2_ERROR   ("Test log level: LOG_2_ERROR   , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_1_FATAL   ("Test log level: LOG_1_FATAL   , test int: " << testInt++ << ", test string: "<< testStr);
	LOG_0_NOFILTER("Test log level: LOG_0_NOFILTER, test int: " << testInt++ << ", test string: "<< testStr);

	LOG_0_NOFILTER("Below 6 variables are printed at filter level TRACE");
	TRVAR1(testInt);
	TRVAR2(testInt,testStr);
	TRVAR3(testInt,testStr,testReal);
	TRVAR4(testInt,testStr,testReal,testVec);
	TRVAR5(testInt,testStr,testReal,testVec,testMat);
	TRVAR6(testInt,testStr,testReal,testVec,testMat,testComplex);

	LOG_0_NOFILTER("Below is macro TRACE;");
	TRACE;
}

// accepted sinks, separately for each log level:
// "cerr", "cout", "file:name", "sms", "shell:scriptName"
void addSink(std::string sink, int level) {
	std::cerr << "FIXME: addSink: " << sink << " level: " << level << "\n";
}

// removes all sinks for selected log level. Attention: even 'cerr' sink is removed!
void clearSink(int level) {
	std::cerr << "FIXME: clearSink level: " << level << "\n";
}

// resets all sinks to default values: 'cerr' for those below logLevel, 'none' for those above logLevel
void resetAllSinks() {
	std::cerr << "FIXME: resetAllSinks\n";
}

void setLevel(std::string className, int level) {
//FIXME: use some #ifdef YADE_BOOST_LOG
	Logging::instance().setNamedLogLevel(className , level);
	LOG_INFO("filter log level for "<<className<<" has been set to " << Logging::instance().getNamedLogLevel(className));
}

py::dict getLevels() {
	py::dict ret{};
//	for(const auto& a : Logging::instance().classLogLevels) {
//		if(a->second != -1) ret[a->first]=a->second;
//	}
	return ret;
}

BOOST_PYTHON_MODULE(_log){
	python::scope().attr("__doc__") = "Access and manipulation of yade logging.";
	YADE_SET_DOCSTRING_OPTS;
// We can use C++ string literal just like """ """ in python to write docstrings (see. https://en.cppreference.com/w/cpp/language/string_literal )
// The """ is a custom delimeter, we could use    R"RAW( instead, or any other delimeter. This decides what will be the termination delimeter.
	py::def("testAllLevels", testAllLevels, R"""(
.. warning:: I must write docstring here!
	)""");
	py::def("addSink", addSink, R"""(
.. warning:: I must write docstring here!
	)""");
	py::def("clearSink", clearSink, R"""(
.. warning:: I must write docstring here!
	)""");
	py::def("resetAllSinks", resetAllSinks, R"""(
.. warning:: I must write docstring here!
	)""");
	py::def("setLevel", setLevel , R"""(
.. warning:: I must write docstring here!
	)""");
	py::def("getLevels", getLevels , R"""(
.. warning:: I must write docstring here!
	)""");

	python::scope().attr("TRACE")=int(6)
	python::scope().attr("DEBUG")=int(5)
	python::scope().attr("INFO")= int(4)
	python::scope().attr("WARN")= int(3)
	python::scope().attr("ERROR")=int(2)
	python::scope().attr("FATAL")=int(1)
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
