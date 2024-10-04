/*****************************************************************************
* @file block.h
*
* @brief Implements a basic block object.
*****************************************************************************/
#ifndef TRAINS_TESTS_CTC_MODEL_BLOCK_H
#define TRAINS_TESTS_CTC_MODEL_BLOCK_H

#include <string>
#include "types.h"

class Block
{
    private:
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
    public:
        Block(
            types::BlockId block_id,
            int block_number,
            int block_length,
            double block_grade,
            int speed_limit,
            bool station,
            std::string station_name,
            bool rail_switch,
            bool underground,
            double elevation,
            double cum_elevation,
            int time_to_travel_block,
            int accel_and_decell,
            int constant_speed_time,
            int total_time,
            int dwell_time,
            double total_time_to_station_with_dwell
        );

        types::BlockId getBlockId(void) const;
        int getBlockNumber(void) const;
        int getBlockLength(void) const;
        double getBlockGrade(void) const;
        int getSpeedLimit(void) const;
        bool isStation(void) const;
        std::string getStationName(void) const;
        bool isRailSwitch(void) const;
        bool isUnderground(void) const;
        double getElevation(void) const;
        double getCumElevatiton(void) const;
        int getTimeToTravelBlock(void) const;
        int getAccelAndDecel(void) const;
        int getkSpeedTime(void) const;
        int getTotalTime(void) const;
        int getDwellTime(void) const;
        int getTotalTimeToStationWithDwell(void) const;
};

#endif // TRAINS_TESTS_CTC_MODEL_BLOCK_H