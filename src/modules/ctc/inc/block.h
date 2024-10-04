/*****************************************************************************
* @file block.h
*
* @brief Implements a basic block object.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_BLOCK_H
#define TRAINS_SRC_MODULES_CTC_INC_BLOCK_H

#include <string>
#include "types.h"

struct Block
{
    types::BlockId block_id;
    int block_number;
    int block_length;
    double block_grade;
    int speed_limit;
    bool station_;
    std::string station_name;
    bool rail_switch;
    bool underground;
    double elevation;
    double cum_elevation;
    int time_to_travel_block;
    int accel_and_decell;
    int constant_speed_time;
    int total_time;
    int dwell_time;
    double total_time_to_station_with_dwell;
};

#endif // TRAINS_SRC_MODULES_CTC_INC_BLOCK_H