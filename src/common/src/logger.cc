/* Includes
 ******************************************************************************/

#include "logger.h"

#include <chrono>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

/* Defines
 ******************************************************************************/

#define LOGGER_ANSI_RESET  "\e[0m"
#define LOGGER_ANSI_RED    "\e[0;31m"
#define LOGGER_ANSI_YELLOW "\e[0;33m"
#define LOGGER_ANSI_GREEN  "\e[0;32m"
#define LOGGER_ANSI_BLUE   "\e[0;34m"
#define LOGGER_ANSI_PURPLE "\e[0;35m"

#define LOGGER_ERROR_LABEL   "ERROR"
#define LOGGER_WARNING_LABEL "WARNING"
#define LOGGER_INFO_LABEL    "INFO"
#define LOGGER_DEBUG_LABEL   "DEBUG"
#define LOGGER_VERBOSE_LABEL "VERBOSE"

#define LOGGER_LEVEL_DEFAULT LOGGER_LEVEL_VERBOSE

#define LOGGER_ATTRIBUTE_START_DELIMITER "["
#define LOGGER_ATTRIBUTE_END_DELIMITER   "]"
#define LOGGER_PREFIX_DELIMITER          "|"
#define LOGGER_TRAILING_SPACE            " "

#define LOGGER_MS_WIDTH 3
#define LOGGER_MS_TO_S  1000

/* Globals
 ******************************************************************************/

#ifdef LOGGER
static const char *kLoggerAnsiColorLut[LOGGER_LEVEL_MAX] = {LOGGER_ANSI_RED, LOGGER_ANSI_YELLOW, LOGGER_ANSI_GREEN, LOGGER_ANSI_BLUE, LOGGER_ANSI_PURPLE};
static const char *kLoggerLevelLabelLut[LOGGER_LEVEL_MAX] = {LOGGER_ERROR_LABEL, LOGGER_WARNING_LABEL, LOGGER_INFO_LABEL, LOGGER_DEBUG_LABEL, LOGGER_VERBOSE_LABEL};
#endif // LOGGER

#ifdef LOGGER_LEVEL
static LoggerLevel logger_level = LOGGER_LEVEL;
#else
static LoggerLevel logger_level = LOGGER_LEVEL_DEFAULT;
#endif // LOGGER_LEVEL

/* Function Prototypes
 ******************************************************************************/

#ifdef LOGGER
static inline void LoggerAttribute(std::ostream &out_stream, const char *const attribute, const bool trailing_space = true);
static void LoggerTime(std::ostream &out_stream);
#endif // LOGGER

/* Function Definitions
 ******************************************************************************/

void LoggerSetLevel(const LoggerLevel level)
{
#ifdef LOGGER
    if (LOGGER_LEVEL_MAX > level)
    {
        logger_level = level;
    }
#else
    LOGGER_UNUSED(level);
#endif // LOGGER
}

LoggerLevel LoggerGetLevel(void)
{
    return logger_level;
}

void LoggerPrefix(std::ostream &out_stream, const LoggerLevel level, const std::string &tag)
{
#ifdef LOGGER
    out_stream << kLoggerAnsiColorLut[level];
    LoggerTime(out_stream);
    LoggerAttribute(out_stream, kLoggerLevelLabelLut[level]);
    LoggerAttribute(out_stream, tag.c_str());
    out_stream << LOGGER_PREFIX_DELIMITER << LOGGER_TRAILING_SPACE;
#else
    LOGGER_UNUSED(out_stream);
    LOGGER_UNUSED(level);
    LOGGER_UNUSED(tag);
#endif // LOGGER
}

void LoggerPostfix(std::ostream &out_stream)
{
#ifdef LOGGER
    out_stream << LOGGER_ANSI_RESET << std::endl;
#else
    LOGGER_UNUSED(out_stream);
#endif // LOGGER
}

#ifdef LOGGER
static inline void LoggerAttribute(std::ostream &out_stream, const char *const attribute, const bool trailing_space)
{
    out_stream << LOGGER_ATTRIBUTE_START_DELIMITER << attribute << LOGGER_ATTRIBUTE_END_DELIMITER;

    if (trailing_space)
    {
        out_stream << LOGGER_TRAILING_SPACE;
    }
}

static void LoggerTime(std::ostream &out_stream)
{
    std::ostringstream out_string_stream;

    // Get current time from the system clock
    const std::chrono::time_point now = std::chrono::system_clock::now();

    // Get local time representation
    std::time_t now_time  = std::chrono::system_clock::to_time_t(now);
    std::tm     localtime = *std::localtime(&now_time);

    // Get milliseconds
    const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % LOGGER_MS_TO_S;

    // Display time in HH:MM:SS.mmm format
    out_string_stream << std::put_time(&localtime, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(LOGGER_MS_WIDTH) << ms.count();
    LoggerAttribute(out_stream, out_string_stream.str().c_str());
}
#endif // LOGGER