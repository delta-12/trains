/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include <unordered_map>
#include <algorithm>
#include <filesystem>

#include "types.h"
#include "block_builder.h"
#include "wayside_controller.h"
#include "wayside_controller_gateway.h"



namespace ctc
{

typedef struct DestinationAndArrivalTime DestinationAndArrivalTime;
typedef struct Train Train;
typedef struct Station Station;

typedef enum
{
    AUTOMATIC_MODE,
    MANUAL_MODE,
    MAINTENANCE_MODE
} OperationMode;

struct Station
{
    Station(void) : station_name(""), block_id(0) {};
    Station(std::string station_name, types::BlockId block_id) : station_name(station_name), block_id(block_id) {};
    std::string station_name;
    types::BlockId block_id;
};

struct DestinationAndArrivalTime
{
    DestinationAndArrivalTime(void);
    DestinationAndArrivalTime(const Station station, types::Tick arrival_time) : station(station), arrival_time(arrival_time) {};
    ctc::Station station;
    types::Tick arrival_time;
};

// 
struct Train
{
    Train(void) : train_id(GetNextId()) {};
    Train(std::string train_name) : train_name(train_name) {};
    types::TrainId train_id;
    std::string train_name;
    std::vector<types::BlockId> block_occupancy;
    types::MetersPerSecond suggested_speed;
    std::vector<types::Block> authority;
    std::vector<DestinationAndArrivalTime> destination_list;
    static types::TrainId last_id;

    static types::TrainId GetNextId() {
        static types::TrainId last_id = 1;
        return last_id++;
    }
};

class Ctc
{
    public:
        void SetSchedule(const types::TrainId train, const std::vector<DestinationAndArrivalTime> &schedule);
        void LoadSchedule(const std::string &file_path);
        types::Error OpenFileExplorer(std::string &file);
        types::Error ManualDispatch(std::string &arrival_time, std::string &station_name, std::string &train_name, types::TrackId track_id);
        types::Error SetBlockStates(const types::TrackId track, const std::vector<wayside_controller::BlockState> &block_states);
        std::vector<wayside_controller::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const;

        /* Helpers */
         
        /* Setters */
        void SetManualMode(void);
        void SetAutomaticMode(void);

        /* Train Specific */
        void UpdateSuggestedSpeed(const types::TrainId train_id);
        void UpdateAuthority(const types::TrainId train_id);
        std::vector<types::Block> GetRoute(const types::BlockId start, const types::BlockId end);

        /* Getters */
        ctc::Train GetTrainByName(const std::string train_name);
        ctc::Train GetTrain(const std::string &train_name);
        types::Block GetBlock(const types::BlockId &block_id);
        ctc::Station GetStationBlockId(const std::string &station_name);
        /* Variables */
        std::vector<types::Block> blocks_;
        std::vector<ctc::Station> stations_;
        std::vector<ctc::Train> train_schedules_;
        OperationMode ctc_mode_;
        std::filesystem::path schedule_file_path;

};

std::vector<std::string> SplitBySemicolon(const std::string& input);

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H