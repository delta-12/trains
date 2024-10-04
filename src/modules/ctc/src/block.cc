#include "block.h"
#include <iostream>

#include "types.h"

Block::Block(
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
    ) {
    block_id_ = block_id;
    block_number_ = block_number;
    block_length_ = block_length;
    block_grade_ = block_grade;
    speed_limit_ = speed_limit;
    station_ = station;
    station_name_ = station_name;
    rail_switch_ = rail_switch;
    underground_ = underground;
    elevation_ = elevation;
    cum_elevation_ = cum_elevation;
    time_to_travel_block_ = time_to_travel_block;
    accel_and_decell_ = accel_and_decell;
    constant_speed_time_ = constant_speed_time;
    total_time_ = total_time;
    dwell_time_ = dwell_time;
    total_time_to_station_with_dwell_ = total_time_to_station_with_dwell;
}

types::BlockId Block::getBlockId(void) const {
    return block_id_;
}

int Block::getBlockNumber(void) const {
    return block_number_;
}

int Block::getBlockLength(void) const {
    return block_length_;
}

double Block::getBlockGrade(void) const {
    return block_grade_;
}

int Block::getSpeedLimit(void) const {
    return speed_limit_;
}

bool Block::isStation(void) const {
    return station_;
}

std::string Block::getStationName(void) const {
    return station_name_;
}

bool Block::isRailSwitch(void) const {
    return rail_switch_;
}

bool Block::isUnderground(void) const {
    return underground_;
}

double Block::getElevation(void) const {
    return elevation_;
}

double Block::getCumElevatiton(void) const {
    return cum_elevation_;
}

int Block::getTimeToTravelBlock(void) const {
    return time_to_travel_block_;
}

int Block::getAccelAndDecel(void) const {
    return accel_and_decell_;
}

int Block::getkSpeedTime(void) const {
    return constant_speed_time_;
}

int Block::getTotalTime(void) const {
    return total_time_;
}

int Block::getDwellTime(void) const {
    return dwell_time_;
}

int Block::getTotalTimeToStationWithDwell(void) const {
    return total_time_to_station_with_dwell_;
}
