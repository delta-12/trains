/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include "types.h"
#include "wayside_controller_gateway.h"
#include "unordered_map"

namespace ctc
{

struct Infrastructure
{
    bool station             = false;
    std::string station_name = "";
    bool railway_crossing    = false;
    bool rail_switch         = false;
    bool underground         = false;
    bool transponder         = false;
};

struct Block
{
    types::BlockId block_id;
    char section;
    int block_length;
    double block_grade;
    int speed_limit;
    double elevation;
    Infrastructure infra;
    double cum_elevation;
    bool occupancy;
    // int time_to_travel_block;
    // int accel_and_decell;
    // int constant_speed_time;
    // int total_time;
    // int dwell_time;
    // double total_time_to_station_with_dwell;
};

class Ctc
{
    public:
        // types::Error UpdateWaysideControllers(wayside_controller::Gateway &gateway);

        ctc::Block PopulateBlockFromRecord(std::vector<std::string> record);

        void PopulateTrainSchedule(std::vector<std::string> record);
        void PrintAllTrainSchedule(void) const;
        void PrintAllBlockMap(void) const;
        void PrintBlockInfo(const ctc::Block& block) const;
        size_t PrintTrackMapSize(void) const;
        void PopulateDataFromCsv(std::vector<std::vector<std::string> > records);
        void AddTrainScheduleManual(std::string& train_name, std::string& destination, std::string& arrival_time);
        void SetBlockOccupancy(std::string block_number);
        std::unordered_map<std::string, ctc::Block> block_map;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string> > train_schedule;

    private:

};

int AssignInfrastructure(const std::string& input, ctc::Block& block);
std::vector<std::string> SplitBySemicolon(const std::string& input);

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H