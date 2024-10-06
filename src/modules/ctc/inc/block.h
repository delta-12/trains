/*****************************************************************************
* @file block.h
*
* @brief Implements a basic block object.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_BLOCK_H
#define TRAINS_SRC_MODULES_CTC_INC_BLOCK_H

#include <chrono>
#include <string>

#include "types.h"

struct Block
{
    types::BlockId id;
    types::Meters length;
    double grade;
    types::MetersPerSecond speed_limit;
    bool has_station;
    std::string station_name;
    bool rail_switch;
    bool underground;
    types::Meters elevation;
    types::Meters cum_elevation;
    std::chrono::seconds time_to_travel_block;
    types::MetersPerSecondSquared acceleration;
    std::chrono::seconds constant_speed_time;
    std::chrono::seconds total_time;
    std::chrono::seconds dwell_time;
    std::chrono::seconds total_time_to_station_with_dwell;
};

#endif // TRAINS_SRC_MODULES_CTC_INC_BLOCK_H