// 2006-2008 © Václav Šmilauer <eudoxos@arcig.cz>
// 2019        Janek Kozicki
#pragma once
/*
 * This file defines various useful logging-related macros - userspace stuff is
 * - LOG_* for actual logging,
 * - DECLARE_LOGGER; that should be used in class header to create separate logger for that class,
 * - CREATE_LOGGER(ClassName); that must be used in class implementation file to create the static variable.
 * - CREATE_CPP_LOCAL_LOGGER("filename.cpp"); use this inside a *.cpp file which has code that does not belong to any class, and needs logging. The name will be used for filtering logging.
 *
 * Note that the latter 2 may change their name to something like LOG_DECLARE and LOG_CREATE, to be consistent.
 * Some other macros will be very likely added, to allow for easy variable tracing etc. Suggestions welcome.
 *
 *
 * Yade has the logging config file by default in ~/.yade-$VERSION/logging.conf.
 *
 */

// boost::log inspired by git 014b11496
#ifdef YADE_BOOST_LOG
	#include <string>
	#include <map>
	#include <lib/base/Singleton.hpp>
	#include <boost/log/expressions.hpp>
	#include <boost/log/trivial.hpp>
	#include <boost/log/utility/setup.hpp>

	#define _LOG_HEAD ":"<<__LINE__<<" "<<__PRETTY_FUNCTION__<<": "
	// If you get "error: ‘logger’ was not declared in this scope" then you have to declare logger.
	// Use DECLARE_LOGGER; inside class and CREATE_LOGGER(ClassName); inside .cpp file
	// or use CREATE_CPP_LOCAL_LOGGER("filename.cpp") if you need logging outside some class.
	#define LOG_TRACE(msg)    { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eTRACE) << _LOG_HEAD << msg; }
	#define LOG_DEBUG(msg)    { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eDEBUG) << _LOG_HEAD << msg; }
	#define LOG_INFO(msg)     { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eINFO)  << _LOG_HEAD << msg; }
	#define LOG_WARN(msg)     { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eWARN)  << _LOG_HEAD << msg; }
	#define LOG_ERROR(msg)    { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eERROR) << _LOG_HEAD << msg; }
	#define LOG_FATAL(msg)    { BOOST_LOG_SEV(logger, Logging::SeverityLevel::eFATAL) << _LOG_HEAD << msg; }
	#define LOG_NOFILTER(msg) { boost::log::sources::severity_logger< Logging::SeverityLevel > slg; BOOST_LOG_SEV(slg, Logging::eNOFILTER ) << _LOG_HEAD << msg; };

	class Logging : public Singleton<Logging> {
		public:
			enum SeverityLevel { eNOFILTER=0, eFATAL=1, eERROR=2, eWARN=3, eINFO=4, eDEBUG=5, eTRACE=6 };
			Logging();
			void        readConfigFile(const std::string&);
			void        setUseColors(bool);
			void        setDefaultLogLevel(short int);
			short int   getDefaultLogLevel() { return defaultLogLevel;};
			short int   getNamedLogLevel  (const std::string&);
			void        setNamedLogLevel  (const std::string&,short int);
			void        unsetNamedLogLevel(const std::string&);
			boost::log::sources::severity_logger< Logging::SeverityLevel > createNamedLogger(std::string name);
			static constexpr short int                maxLogLevel{MAX_LOG_LEVEL};
		private:
			typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
			std::map<std::string,short int>::iterator   findFilterName(const std::string&);
			short int                                   defaultLogLevel{4};
			std::map<std::string,short int>             classLogLevels{{"Default",4}};
			boost::shared_ptr< text_sink >              sink{boost::make_shared< text_sink >()};
			bool                                        colors{true};
		FRIEND_SINGLETON(Logging);
	};
	BOOST_LOG_ATTRIBUTE_KEYWORD(severity      , "Severity" , Logging::SeverityLevel )
	BOOST_LOG_ATTRIBUTE_KEYWORD(class_name_tag, "NameTag"  , std::string            )
	inline std::ostream& operator<< (std::ostream& strm, Logging::SeverityLevel level) // necessary for formatting output.
	{
		static const char* strings[] = { "NOFILTER" , "FATAL_1" , "ERROR_2" , "WARN__3" , "INFO__4" , "DEBUG_5" , "TRACE_6" };
		if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
			strm << strings[level];
		else
			strm << static_cast< int >(level);
		return strm;
	}
	// logger is local for every class, but if it is missing, we will use the parent's class logger automagically.
	#define DECLARE_LOGGER public: static boost::log::sources::severity_logger< Logging::SeverityLevel > logger;
	#define CREATE_LOGGER(classname) boost::log::sources::severity_logger< Logging::SeverityLevel > classname::logger=Logging::instance().createNamedLogger(#classname);
	#define CREATE_CPP_LOCAL_LOGGER(filtername) namespace{ boost::log::sources::severity_logger< Logging::SeverityLevel > logger=Logging::instance().createNamedLogger(filtername); };
#else
	#include <iostream>
	#define _POOR_MANS_LOG(level,msg) {std::cerr<<level " "<<_LOG_HEAD<<msg<<std::endl;}
	#define _LOG_HEAD __FILE__ ":"<<__LINE__<<" "<<__PRETTY_FUNCTION__<<": "

	#ifdef YADE_DEBUG
	// when compiling with debug symbols and without boost::log it will print everything.
		#define MAX_HARDCODED_LOG_LEVEL 6
		#define LOG_TRACE(msg) _POOR_MANS_LOG("TRACE_6",msg)
		#define LOG_DEBUG(msg) _POOR_MANS_LOG("DEBUG_5",msg)
		#define LOG_INFO(msg)  _POOR_MANS_LOG("INFO__4",msg)
	#else
		#define MAX_HARDCODED_LOG_LEVEL 3
		#define LOG_TRACE(msg)
		#define LOG_DEBUG(msg)
		#define LOG_INFO(msg)
	#endif

	#define LOG_WARN(msg)     _POOR_MANS_LOG("WARN__3",msg)
	#define LOG_ERROR(msg)    _POOR_MANS_LOG("ERROR_2",msg)
	#define LOG_FATAL(msg)    _POOR_MANS_LOG("FATAL_1",msg)
	#define LOG_NOFILTER(msg) _POOR_MANS_LOG("NOFILTER",msg)

	#define DECLARE_LOGGER
	#define CREATE_LOGGER(classname)
	#define CREATE_CPP_LOCAL_LOGGER(name)
#endif

// macros for quick debugging
#define TRACE LOG_TRACE("Been here")
#define _TRV(x) #x"="<<x<<"; "
#define TRVAR1(a) LOG_TRACE( _TRV(a) );
#define TRVAR2(a,b) LOG_TRACE( _TRV(a) << _TRV(b) );
#define TRVAR3(a,b,c) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) );
#define TRVAR4(a,b,c,d) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) << _TRV(d) );
#define TRVAR5(a,b,c,d,e) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) << _TRV(d) << _TRV(e) );
#define TRVAR6(a,b,c,d,e,f) LOG_TRACE( _TRV(a) << _TRV(b) << _TRV(c) << _TRV(d) << _TRV(e) << _TRV(f) );

// Logger aliases:
#define LOG_6_TRACE(msg) LOG_TRACE(msg)
#define LOG_5_DEBUG(msg) LOG_DEBUG(msg)
#define LOG_4_INFO(msg)  LOG_INFO(msg)
#define LOG_3_WARN(msg)  LOG_WARN(msg)
#define LOG_2_ERROR(msg) LOG_ERROR(msg)
#define LOG_1_FATAL(msg) LOG_FATAL(msg)
#define LOG_0_NOFILTER(msg) LOG_NOFILTER(msg)

#define LOG_6(msg) LOG_TRACE(msg)
#define LOG_5(msg) LOG_DEBUG(msg)
#define LOG_4(msg) LOG_INFO(msg)
#define LOG_3(msg) LOG_WARN(msg)
#define LOG_2(msg) LOG_ERROR(msg)
#define LOG_1(msg) LOG_FATAL(msg)
#define LOG_0(msg) LOG_NOFILTER(msg)

// honor MAX_LOG_LEVEL cmake option to disable selected macros
#if MAX_LOG_LEVEL<6
	#undef LOG_TRACE
	#define LOG_TRACE(msg)
#endif

#if MAX_LOG_LEVEL<5
	#undef LOG_DEBUG
	#define LOG_DEBUG(msg)
#endif

#if MAX_LOG_LEVEL<4
	#undef LOG_INFO
	#define LOG_INFO(msg)
#endif

#if MAX_LOG_LEVEL<3
	#undef LOG_WARN
	#define LOG_WARN(msg)
#endif

#if MAX_LOG_LEVEL<2
	#warning "MAX_LOG_LEVEL<2 means that all LOG_ERROR messages are ignored, be careful with this option."
	#undef LOG_ERROR
	#define LOG_ERROR(msg)
#endif

#if MAX_LOG_LEVEL<1
	#warning "MAX_LOG_LEVEL<1 means that all LOG_ERROR, LOG_FATAL messages are ignored, be careful with this option."
	#undef LOG_FATAL
	#define LOG_FATAL(msg)
#endif

#if MAX_LOG_LEVEL<0
	#warning "MAX_LOG_LEVEL<0 means that all LOG_ERROR, LOG_FATAL and LOG_NOFILTER messages are are ignored, be careful with this option."
	#undef LOG_NOFILTER
	#define LOG_NOFILTER(msg)
#endif

