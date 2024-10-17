/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include <unordered_map>
#include <algorithm>

#include "types.h"
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
    Train(void) : train_id(++last_id) {};
    Train(std::string train_name) : train_name(train_name), train_id(++last_id) {};
    types::TrainId train_id;
    types::TrackId track_id;
    std::string train_name;
    std::vector<types::BlockId> block_occupancy;
    types::MetersPerSecond suggested_speed;
    std::vector<types::Block> authority;
    std::vector<DestinationAndArrivalTime> destination_list;
    static types::TrainId last_id;
};

class Line
{  
    public:
        Line(void);
        Line(const std::vector<types::Block> &block_list);
        std::unordered_map<types::BlockId, types::Block> block_map_;
        std::vector<ctc::Station> station_list_;
        types::TrackId track_;

        types::Block GetBlock(const types::BlockId blockId );
        Station GetStationByStationName(const std::string station_name );
};

class Ctc
{
    public:
        void SetTrackLayout(const std::vector<types::Block> &blocks);
        void SetSchedule(const types::TrainId train, const std::vector<DestinationAndArrivalTime> &schedule);
        void SetManualMode(void);

        void ManualDispatch(std::string &arrival_time, std::string &station_name, std::string &train_name, types::TrackId track_id);
        types::Error SetBlockStates(const types::TrackId track, const std::vector<wayside_controller::BlockState> &block_states);
        std::vector<wayside_controller::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const;

        ctc::Train GetTrainByName(const std::string train_name);
        ctc::Line GetLine(const types::TrackId track_id);

        /* Variables */
        std::unordered_map<types::TrackId, ctc::Line> track_layout_;
        std::unordered_map<types::TrainId, ctc::Train> train_schedule_;
        OperationMode ctc_mode_;

};

std::vector<std::string> SplitBySemicolon(const std::string& input);

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H