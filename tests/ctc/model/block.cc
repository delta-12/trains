#include "block.h"

#include <string>
#include <iostream>

using namespace std;

Block::Block(
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
) {
    this->block_id = block_id;
    this->block_number = block_number;
    this->block_length = block_length;
    this->block_grade = block_grade;
    this->station = station;
    this->station_name = station_name;
    this->rail_switch = rail_switch;
    this->underground = underground;
    this->elevation = elevation;
    this->cumulative_elevation = cumulative_elevation;
    this->time_to_travel_block = time_to_travel_block;
    this->accel_and_decel = accel_and_decel;
    this->k_speed_time = k_speed_time;
    this->total_time = total_time;
    this->dwell_time = dwell_time;
    this->total_time_to_station_with_dwell = total_time_to_station_with_dwell;
}

int Block::getBlockNumber() {
    return block_number;
}

int Block::getBlockLength() {
    return block_length;
}

double Block::getBlockGrade() {
    return block_grade;
}

int Block::getSpeedLimit() {
    return speed_limit;
}

bool Block::isStation() {
    return station;
}

string Block::getStationName() {
    return station_name;
}

bool Block::isRailSwitch() {
    return rail_switch;
}

bool Block::isUnderground() {
    return underground;
}

double Block::getElevation() {
    return elevation;
}

double Block::getCumElevatiton() {
    return cumulative_elevation;
}

int Block::getTimeToTravelBlock() {
    return time_to_travel_block;
}

int Block::getAccelAndDecel() {
    return accel_and_decel;
}

int Block::getkSpeedTime() {
    return k_speed_time;
}

int Block::getTotalTime() {
    return total_time;
}

int Block::getDwellTime() {
    return dwell_time;
}

int Block::getTotalTimeToStationWithDwell() {
    return total_time_to_station_with_dwell;
}
