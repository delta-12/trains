/*****************************************************************************
 * @file block.h
 *
 * @brief Defines block objects
 *****************************************************************************/
#ifndef TRAINS_TESTS_CTC_MODEL_BLOCK_H
#define TRAINS_TESTS_CTC_MODEL_BLOCK_H

#include "types.h"

using namespace std;

class Block
{
    private:
        types::BlockId block_id;
        int block_number;
        int block_length;
        double block_grade;
        int speed_limit;
        bool station;
        string station_name;
        bool rail_switch;
        bool underground;
        double elevation;
        double cumulative_elevation;
        int time_to_travel_block;
        int accel_and_decel;
        int k_speed_time;
        int total_time;
        int dwell_time;
        int total_time_to_station_with_dwell;
    public:
        Block(
            types::BlockId block_id,
            int block_number,
            int block_length,
            double block_grade,
            int speed_limit,
            bool station,
            string station_name,
            bool rail_switch,
            bool underground,
            double elevation,
            double cumulative_elevation,
            int time_to_travel_block,
            int accel_and_decel,
            int k_speed_time,
            int total_time,
            int dwell_time,
            int total_time_to_station_with_dwell
        );

        types::BlockId getBlockId();
        int getBlockNumber();
        int getBlockLength();
        double getBlockGrade();
        int getSpeedLimit();
        bool isStation();
        string getStationName();
        bool isRailSwitch();
        bool isUnderground();
        double getElevation();
        double getCumElevatiton();
        int getTimeToTravelBlock();
        int getAccelAndDecel();
        int getkSpeedTime();
        int getTotalTime();
        int getDwellTime();
        int getTotalTimeToStationWithDwell();
};

#endif // TRAINS_TESTS_CTC_MODEL_BLOCK_H