#ifndef __LOGGER_DEFINES_HPP__
#define __LOGGER_DEFINES_HPP__

/* https://github.com/orocos-toolchain/log4cpp */
/* License LGPL 2.1 */
#include <log4cpp/Category.hh>

#define LOG_DEBUG			log4cpp::Category::getRoot().debug
#define LOG_INFO			log4cpp::Category::getRoot().info
#define LOG_WARNING			log4cpp::Category::getRoot().warn
#define LOG_ERROR			log4cpp::Category::getRoot().error

#define LOG_DEBUG_SS		log4cpp::Category::getRoot().debugStream()
#define LOG_INFO_SS			log4cpp::Category::getRoot().infoStream()
#define LOG_WARNING_SS		log4cpp::Category::getRoot().warnStream()
#define LOG_ERROR_SS		log4cpp::Category::getRoot().errorStream()

#define WLOG_DEBUG			log4cpp::Category::getRoot().debugW
#define WLOG_INFO			log4cpp::Category::getRoot().infoW
#define WLOG_WARNING		log4cpp::Category::getRoot().warnW
#define WLOG_ERROR			log4cpp::Category::getRoot().errorW

#define LOG_ERROR_MSG		"%s> %s", __FUNCTION__, AnsiString(E->Message).c_str()

#endif // __LOGGER_DEFINES_HPP__
