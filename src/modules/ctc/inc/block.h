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
    types::BlockId block_id_;
    int block_number_;
    int block_length_;
    double block_grade_;
    int speed_limit_;
    bool station_;
    std::string station_name_;
    bool rail_switch_;
    bool underground_;
    double elevation_;
    double cum_elevation_;
    int time_to_travel_block_;
    int accel_and_decell_;
    int constant_speed_time_;
    int total_time_;
    int dwell_time_;
    double total_time_to_station_with_dwell_;
};

#endif // TRAINS_SRC_MODULES_CTC_INC_BLOCK_H