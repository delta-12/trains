/*****************************************************************************
* @file tick_source.h
*
* @brief Provides a single source of time with millisecond precision and a
         variable rate.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_TICK_SOURCE_H
#define TRAINS_SRC_COMMON_INC_TICK_SOURCE_H

#include <chrono>
#include <string>

#include "types.h"

class TickSource
{
public:
    TickSource(void);
    TickSource(const std::string &hours_minutes_seconds);
    TickSource(const std::chrono::system_clock::time_point time);
    TickSource(const std::chrono::milliseconds tick_duration);
    TickSource(const std::string &hours_minutes_seconds, const std::chrono::milliseconds tick_duration);
    TickSource(const std::chrono::system_clock::time_point, const std::chrono::milliseconds tick_duration);
    void Start(void);
    void Stop(void);
    bool GetState(void) const;
    types::Error SetTime(const std::string &hours_minutes_seconds);
    void SetTime(const std::chrono::system_clock::time_point);
    void SetTickDuration(const std::chrono::milliseconds tick_duration);
    void SetMultiplier(const uint8_t multiplier);
    std::chrono::milliseconds GetTickDuration(void) const;
    uint8_t GetMultiplier(void) const;
    types::Tick GetTick(void) const;
    types::Tick GetElapsedTicks(const types::Tick start) const;
    types::Tick GetElapsedTicks(const types::Tick start, const types::Tick end) const;
    std::chrono::system_clock::time_point GetTime(void) const;
    std::string GetTimeString(void) const;
    std::chrono::milliseconds GetElapsedTime(const types::Tick start) const;
    std::chrono::milliseconds GetElapsedTime(const types::Tick start, const types::Tick end) const;

private:
    void Initialize(const std::chrono::system_clock::time_point, const std::chrono::milliseconds tick_duration);
    types::Error GetTimePoint(const std::string &hours_minutes_seconds, std::chrono::system_clock::time_point &time_point) const;
    std::chrono::milliseconds GetDuration(const types::Tick ticks) const;

    types::Tick tick_;
    std::chrono::milliseconds tick_duration_;
    uint8_t multiplier_;
    std::chrono::system_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_time_;
    bool running_;
};

#endif // TRAINS_SRC_COMMON_INC_TICK_SOURCE_H