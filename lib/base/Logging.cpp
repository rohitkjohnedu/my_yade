// 2019        Janek Kozicki

#ifdef YADE_BOOST_LOG

#include <ostream>
#include <lib/base/Logging.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

SINGLETON_SELF(Logging);

bool logFilterLevels(  boost::log::value_ref< Logging::SeverityLevel , tag::severity      > const& level
		     , boost::log::value_ref< std::string            , tag::class_name_tag> const& name )
{
/*
	auto itEnd          = Logging::instance().classLogLevels.end();
	short int itDefault = Logging::instance().getDefaultLogLevel();
	if(itDefault == itEnd) { // something is seriously broken, we resort to std::cerr to report that.
		std::cerr << "LOGGER Warning: \"Default\" log level is missing. Expect problems with logging.\n";
		return true;
	}
	if(not level) { // something is seriously broken, we resort to std::cerr to report that.
		std::cerr << "LOGGER Warning: Logging::SeverityLevel is missing. Expect problems with logging.\n";
		return true;
	}
	if(name) {
		auto it = Logging::instance().classLogLevels.find(name.get());
		if( ( it != itEnd ) and ( it->second >= 0 ) ) {
			return level <= it->second;
		}
	} else {
		std::cerr << "LOGGER Warning: class_name_tag needed for filtering is missing. Expect problems with logging.\n";
	}
	return level <= itDefault->second;
*/
	return true;
}

// Setup the common formatter for all sinks
Logging::Logging() : defaultLogLevel(4), classLogLevels{{"Default",defaultLogLevel}}, sink{boost::make_shared< text_sink >()}, colors{true} {
	boost::log::formatter fmt = boost::log::expressions::stream
		<< "<" << severity << "> "
		<< boost::log::expressions::if_(boost::log::expressions::has_attr(class_name_tag))
		[
			boost::log::expressions::stream << class_name_tag
		]
		<< boost::log::expressions::smessage;

	boost::shared_ptr< std::ostream > stream(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream);
	sink->set_formatter(fmt);
	sink->set_filter( boost::phoenix::bind(&logFilterLevels, severity.or_none(), class_name_tag.or_none() ));
	boost::log::core::get()->add_sink(sink);
}

void Logging::readConfigFile(const std::string& fname) {
	// TODO ...
}

void Logging::setDefaultLogLevel(short int level) {
	classLogLevels["Default"] = level;
	defaultLogLevel           = level;
}

short int Logging::getNamedLogLevel  (const std::string& name) {
	return findFilterName(name)->second;
}

void Logging::setNamedLogLevel  (const std::string& name , short int level) {
	if(level > maxLogLevel) {
		std::cerr << "LOGGER Warning: setting \""<<name<<"\" log level higher than MAX_LOG_LEVEL="<<maxLogLevel<<" will have no effect. Logs will not be printed, they were removed during compilation.\n";
		std::cerr << "LOGGER Warning: to be able to use \""<<name<<"\"="<<level<<" you have to recompile yade with cmake option MAX_LOG_LEVEL="<<level<<" or higher.\n";
	}
	if( name == "Default" ) {
		setDefaultLogLevel(level);
	} else {
		findFilterName(name)->second = level;
	}
}

void Logging::unsetNamedLogLevel(const std::string& name) {
	if(name == "Default") {
		// unsetting Default will result in printing everything.
		classLogLevels["Default"] = 8;
	} else {
		// unsetting anything else will result in printing it at Default level.
		findFilterName(name)->second = -1;
	}
}

boost::log::sources::severity_logger< Logging::SeverityLevel > Logging::createNamedLogger(std::string name) {
	boost::log::sources::severity_logger< Logging::SeverityLevel > l;
	l.add_attribute("NameTag", boost::log::attributes::constant< std::string >(name));
	classLogLevels[name] = -1;
	return l;
};

std::map<std::string,short int>::iterator Logging::findFilterName(const std::string& name) {
	auto it = classLogLevels.find(name);
	if( it == classLogLevels.end() ) {
		throw std::runtime_error(name+" is not recognized. Did you forget CREATE_LOGGER; and DECLARE_LOGGER(Classname); macros? Or maybe CREATE_CPP_LOCAL_LOGGER(\"filename.cpp\"); macro?\n");
	}
	return it;
}

#endif

