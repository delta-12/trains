/*****************************************************************************
*  @file       Logger.h
*
*  @brief      Minimal ANSI C++ logger.
*****************************************************************************/

#ifndef TRAINS_SRC_COMMON_INC_LOGGER_H
#define TRAINS_SRC_COMMON_INC_LOGGER_H

/* Includes
 ******************************************************************************/

#include <format>
#include <ostream>
#include <string>

/* Defines
 ******************************************************************************/

#define LOGGER_UNUSED(arg) (void)(arg)

/* Typedefs
 ******************************************************************************/

typedef enum
{
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_VERBOSE,
    LOGGER_LEVEL_MAX
} LoggerLevel;

/* Function Prototypes
 ******************************************************************************/

template <typename ... Args>
void LoggerLog(std::ostream &out_stream, const LoggerLevel level, const std::string &tag, const std::format_string<Args...> &format_string, Args &&... args);
void LoggerSetLevel(const LoggerLevel level);
LoggerLevel LoggerGetLevel(void);
void LoggerPrefix(std::ostream &out_stream, const LoggerLevel level, const std::string &tag);
void LoggerPostfix(std::ostream &out_stream);

#ifdef LOGGER
#define LOGGER_LOG_ERROR(out_stream, tag, format_string, ...)   LoggerLog(out_stream, LOGGER_LEVEL_ERROR, tag, format_string, ## __VA_ARGS__)
#define LOGGER_LOG_WARNING(out_stream, tag, format_string, ...) LoggerLog(out_stream, LOGGER_LEVEL_WARNING, tag, format_string, ## __VA_ARGS__)
#define LOGGER_LOG_INFO(out_stream, tag, format_string, ...)    LoggerLog(out_stream, LOGGER_LEVEL_INFO, tag, format_string, ## __VA_ARGS__)
#define LOGGER_LOG_DEBUG(out_stream, tag, format_string, ...)   LoggerLog(out_stream, LOGGER_LEVEL_DEBUG, tag, format_string, ## __VA_ARGS__)
#define LOGGER_LOG_VERBOSE(out_stream, tag, format_string, ...) LoggerLog(out_stream, LOGGER_LEVEL_VERBOSE, tag, format_string, ## __VA_ARGS__)
#else
#define LOGGER_LOG_ERROR(out_stream, tag, format_string, ...)   ;
#define LOGGER_LOG_WARNING(out_stream, tag, format_string, ...) ;
#define LOGGER_LOG_INFO(out_stream, tag, format_string, ...)    ;
#define LOGGER_LOG_DEBUG(out_stream, tag, format_string, ...)   ;
#define LOGGER_LOG_VERBOSE(out_stream, tag, format_string, ...) ;
#endif // LOGGER

/* Function Definitions
 ******************************************************************************/

template <typename ... Args>
void LoggerLog(std::ostream &out_stream, const LoggerLevel level, const std::string &tag, const std::format_string<Args...> &format_string, Args &&... args)
{
#ifdef LOGGER
    if (LoggerGetLevel() >= level)
    {
        LoggerPrefix(out_stream, level, tag);
        out_stream << std::format(format_string, std::forward<Args>(args)...);
        LoggerPostfix(out_stream);
    }
#else
    LOGGER_UNUSED(out_stream);
    LOGGER_UNUSED(level);
    LOGGER_UNUSED(tag);
    LOGGER_UNUSED(format_string);
    (LOGGER_UNUSED(std::forward<Args>(args)), ...);
#endif // LOGGER
}

#endif // TRAINS_SRC_COMMON_INC_LOGGER_H