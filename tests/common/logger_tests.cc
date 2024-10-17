/*****************************************************************************
* @file logger_tests.cc
*
* @brief Unit testing for Logger.
*****************************************************************************/

#include <sstream>

#include <gtest/gtest.h>

#include "logger.h"

#define LOGGER_TEST_ANSI_COLOR_PREFIX "\e[0;"
#define LOGGER_TEST_ANSI_COLOR_RESET  "\e[0m"

TEST(LoggerTests, SetAndGetLevel)
{
    ASSERT_EQ(LOGGER_LEVEL_VERBOSE, LoggerGetLevel());

    LoggerSetLevel(LOGGER_LEVEL_DEBUG);
    ASSERT_EQ(LOGGER_LEVEL_DEBUG, LoggerGetLevel());

    LoggerSetLevel(LOGGER_LEVEL_MAX);
    ASSERT_EQ(LOGGER_LEVEL_DEBUG, LoggerGetLevel());

    LoggerSetLevel(LOGGER_LEVEL_VERBOSE);
    ASSERT_EQ(LOGGER_LEVEL_VERBOSE, LoggerGetLevel());
}

TEST(LoggerTests, LoggerLogTest)
{
    std::stringstream buffer;

    LoggerLog(buffer, LOGGER_LEVEL_MAX, "TAG", "foobar");
    ASSERT_STREQ("", buffer.str().c_str());

    buffer.str(std::string());
    LoggerLog(buffer, LOGGER_LEVEL_VERBOSE, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(40, 11).c_str());
    buffer.str(std::string());
    LoggerLog(buffer, LOGGER_LEVEL_VERBOSE, "TAG", "foobar {}{}{}{}", 0, 1, 2, " baz");
    ASSERT_STREQ("foobar 012 baz", buffer.str().substr(40, 14).c_str());

    LoggerSetLevel(LOGGER_LEVEL_WARNING);
    buffer.str(std::string());
    LoggerLog(buffer, LOGGER_LEVEL_INFO, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("", buffer.str().c_str());
    buffer.str(std::string());
    LoggerLog(buffer, LOGGER_LEVEL_WARNING, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(40, 11).c_str());
    buffer.str(std::string());
    LoggerLog(buffer, LOGGER_LEVEL_ERROR, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(38, 11).c_str());

    LoggerSetLevel(LOGGER_LEVEL_VERBOSE);
    buffer.str(std::string());
    LOGGER_LOG_VERBOSE(buffer, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(40, 11).c_str());
    buffer.str(std::string());
    LOGGER_LOG_DEBUG(buffer, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(38, 11).c_str());
    buffer.str(std::string());
    LOGGER_LOG_INFO(buffer, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(37, 11).c_str());
    buffer.str(std::string());
    LOGGER_LOG_WARNING(buffer, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(40, 11).c_str());
    buffer.str(std::string());
    LOGGER_LOG_ERROR(buffer, "TAG", "foobar {}{}{}{}", 0, 1, 2, 3);
    ASSERT_STREQ("foobar 0123", buffer.str().substr(38, 11).c_str());
}

TEST(LoggerTests, LoggerPrefixTest)
{
    std::stringstream buffer;

    LoggerPrefix(buffer, LOGGER_LEVEL_INFO, "TAG");
    ASSERT_STREQ(LOGGER_TEST_ANSI_COLOR_PREFIX, buffer.str().substr(0, 4).c_str());
    ASSERT_STREQ("[INFO]", buffer.str().substr(22, 6).c_str());
    ASSERT_STREQ("[TAG]", buffer.str().substr(29, 5).c_str());
}

TEST(LoggerTests, LoggerPostfixTest)
{
    std::stringstream buffer;

    LoggerPostfix(buffer);
    ASSERT_STREQ(LOGGER_TEST_ANSI_COLOR_RESET, buffer.str().substr(0, 4).c_str());
    ASSERT_STREQ("\n", buffer.str().substr(4, 1).c_str());
}