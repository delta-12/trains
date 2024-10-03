#include "tick_source.h"

#include <iomanip>
#include <sstream>

TickSource::TickSource(void) : TickSource(std::chrono::system_clock::now(), std::chrono::milliseconds(1))
{
}

TickSource::TickSource(const std::string &hours_minutes_seconds) : TickSource(hours_minutes_seconds, std::chrono::milliseconds(1))
{
}

TickSource::TickSource(const std::chrono::system_clock::time_point time) : TickSource(time, std::chrono::milliseconds(1))
{
}

TickSource::TickSource(const std::chrono::milliseconds tick_duration) : TickSource(std::chrono::system_clock::now(), tick_duration)
{
}

TickSource::TickSource(const std::string &hours_minutes_seconds, const std::chrono::milliseconds tick_duration)
{
    std::chrono::system_clock::time_point time = std::chrono::system_clock::now();

    Initialize(time, tick_duration);
    SetTime(hours_minutes_seconds);
}

TickSource::TickSource(const std::chrono::system_clock::time_point time, const std::chrono::milliseconds tick_duration)
{
    Initialize(time, tick_duration);
}

void TickSource::Start(void)
{
    last_time_ = std::chrono::steady_clock::now();
    running_   = true;
}

void TickSource::Stop(void)
{
    tick_    = GetTick();
    running_ = false;
}

bool TickSource::GetState(void) const
{
    return running_;
}

types::Error TickSource::SetTime(const std::string &hours_minutes_seconds)
{
    std::chrono::system_clock::time_point time;
    types::Error                          error = GetTimePoint(hours_minutes_seconds, time);

    if (types::ERROR_NONE == error)
    {
        SetTime(time);
    }

    return error;
}

void TickSource::SetTime(const std::chrono::system_clock::time_point time)
{
    bool running = running_;

    Stop();
    tick_       = 0; // setting time resets tick count
    start_time_ = time;

    if (running)
    {
        Start();
    }
}

void TickSource::SetTickDuration(const std::chrono::milliseconds tick_duration)
{
    bool running = running_;

    Stop();
    tick_duration_ = tick_duration;

    if (running)
    {
        Start();
    }
}

void TickSource::SetMultiplier(const uint8_t multiplier)
{
    bool running = running_;

    Stop();
    multiplier_ = multiplier;

    if (running)
    {
        Start();
    }
}

std::chrono::milliseconds TickSource::GetTickDuration(void) const
{
    return tick_duration_;
}

uint8_t TickSource::GetMultiplier(void) const
{
    return multiplier_;
}

types::Tick TickSource::GetTick(void) const
{
    types::Tick tick = tick_;

    if (running_)
    {
        tick += (multiplier_ * std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_time_) / tick_duration_);
    }

    return tick;
}

types::Tick TickSource::GetElapsedTicks(const types::Tick start) const
{
    return GetElapsedTicks(start, GetTick());
}

types::Tick TickSource::GetElapsedTicks(const types::Tick start, const types::Tick end) const
{
    return (end - start);
}

std::chrono::system_clock::time_point TickSource::GetTime(void) const
{
    return start_time_ + GetDuration(GetTick());
}

std::string TickSource::GetTimeString(void) const
{
    std::stringstream buffer;
    std::time_t       time      = std::chrono::system_clock::to_time_t(GetTime());
    std::tm           localtime = *std::localtime(&time);

    buffer << std::put_time(&localtime, "%T");

    return buffer.str();
}

std::chrono::milliseconds TickSource::GetElapsedTime(const types::Tick start) const
{
    return GetDuration(GetElapsedTicks(start));
}

std::chrono::milliseconds TickSource::GetElapsedTime(const types::Tick start, const types::Tick end) const
{
    return GetDuration(GetElapsedTicks(start, end));
}

void TickSource::Initialize(const std::chrono::system_clock::time_point time, const std::chrono::milliseconds tick_duration)
{
    tick_          = 0;
    tick_duration_ = tick_duration;
    multiplier_    = 1;
    start_time_    = time;
    last_time_     = std::chrono::steady_clock::now();
    running_       = false;
}

types::Error TickSource::GetTimePoint(const std::string &hours_minutes_seconds, std::chrono::system_clock::time_point &time_point) const
{
    types::Error      error = types::ERROR_NONE;
    std::stringstream buffer;
    struct std::tm    time;

    buffer << hours_minutes_seconds;
    buffer >> std::get_time(&time, "%T");

    if (buffer.fail())
    {
        error = types::ERROR_INVALID_FORMAT;
    }
    else
    {
        // Windows uses a different epoch than Unix systems.  Modifying hours, minutes, seconds of current time ensures compatability across systems.
        const std::time_t now       = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm           localtime = *std::localtime(&now);
        localtime.tm_hour = time.tm_hour;
        localtime.tm_min  = time.tm_min;
        localtime.tm_sec  = time.tm_sec;

        time_point = std::chrono::system_clock::from_time_t(std::mktime(&localtime));
    }

    return error;
}

std::chrono::milliseconds TickSource::GetDuration(const types::Tick ticks) const
{
    return ticks * tick_duration_;
}