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
#include "csv_parser.h"
#include "block_builder.h"
#include "wayside_controller.h"
#include "wayside_controller_gateway.h"
#include "graph.h"

namespace ctc
{

typedef uint16_t                         column;
typedef struct DestinationAndArrivalTime DestinationAndArrivalTime;
typedef struct Train                     Train;
typedef struct Station                   Station;

typedef enum
{
    SectionDirection_Forward,
    SectionDirection_Reverse,
} SectionDirection;

typedef enum
{
    AUTOMATIC_MODE,
    MANUAL_MODE,
    MAINTENANCE_MODE
} CtcOperationMode;

struct Station
{
    Station(void) : station_name(""), block_id(0)
    {
    };
    Station(std::string station_name, types::BlockId block_id, std::chrono::duration<double> t) : station_name(station_name), block_id(block_id), total_time_to_station_with_dwell(t)
    {
    };
    std::string station_name;
    types::BlockId block_id;
    std::chrono::duration<double> total_time_to_station_with_dwell;
};

struct DestinationAndArrivalTime
{
    DestinationAndArrivalTime(void);
    DestinationAndArrivalTime(const Station station, types::Tick arrival_time) : station(station), arrival_time(arrival_time)
    {
    };
    ctc::Station station;
    types::Tick arrival_time;
};

class ScheduleBuilder
{
    public:
        ScheduleBuilder(void);
        ScheduleBuilder(std::filesystem::path &file_path);
};

class GraphBuilder
{
    public:
        GraphBuilder(void);
        GraphBuilder(std::vector<types::Block> &blocks);
        void BuildGreenLine(void);
        std::vector<types::Block> BuildSection(types::SectionId section_id);
        void AddSection(std::vector<types::Block> section, types::BlockDirection direction);

        std::size_t GetBlockList(void) const;
        types::BlockId GetSectionBegin(void) const;
        types::BlockId GetSectionEnd(void) const;
        Graph<types::BlockId, types::Meters> GetGraph(void);

    private:
        std::vector<types::Block> block_list_;
        Graph<types::BlockId, types::Meters> graph_;
};

struct Train
{
    Train(void) : train_id(GetNextId())
    {
    };
    Train(std::string train_name) : train_name(train_name)
    {
    };
    types::TrainId train_id;
    std::string train_name;
    std::vector<types::BlockId> block_occupancy;
    types::BlockId current_position;
    types::MetersPerSecond suggested_speed;
    std::queue<types::BlockId> authority;
    std::vector<DestinationAndArrivalTime> destination_list;
    static types::TrainId last_id;

    static types::TrainId GetNextId()
    {
        static types::TrainId last_id = 1;
        return last_id++;
    }
};

class Ctc
{
    public:
        /* Dispatch */
        void LoadSchedule(void);
        void ManualDispatch(types::BlockId destination);
        types::Error OpenFileExplorer(std::string &file);

        /* Integration */
        types::Error SetBlockStates(const types::TrackId track, const std::vector<wayside_controller::BlockState> &block_states);
        std::vector<wayside_controller::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void);

        /* Train Specific */
        types::Error UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id);
        void AssignAuthority(const std::vector<types::BlockId> &route, types::TrainId train_id);
        std::vector<types::BlockId> FindRoute(const types::BlockId start, const types::BlockId end);
        types::Tick GetTrainDepartureTime(const ctc::Train);
        void AddTrainToTrainSchedule(ctc::Train train);

        /* Block Specific */
        types::Error SetBlockToMaintenanceMode(const types::BlockId block_id);

        /* Getters */
        ctc::Train GetTrainById(const types::TrainId train_id) const;
        ctc::Train* GetTrainPointerById(const types::TrainId train_id);
        types::Block GetBlockById(const types::BlockId block_id);
        ctc::Station GetStationByName(const std::string &station_name);
        std::filesystem::path GetCsvPath(void) const;
        std::vector<types::Block> GetBlocks(void) const;
        std::size_t GetBlockSize(void) const;
        std::size_t GetTrainAuthority(types::TrainId train_id) const;
        std::size_t GetNumStation(void) const;
        std::vector<ctc::Station> GetStations(void) const;

        /* Setters */
        void SetManualMode(void);
        void SetAutomaticMode(void);
        void SetScheduleFilePath(std::filesystem::path path);
        void SetBlocks(std::vector<types::Block> &blocks);
        void SetTrackLayout(void);
        void SetGraphLayout(std::vector<types::Block> &blocks);
        void SetStations(std::vector<types::Block> &blocks);
        void SetGraphManually(Graph<types::BlockId, types::Meters> graph);
        void SetGreenLineGraph(void);

    private:
        /* Variables */
        std::vector<types::Block> blocks_;
        std::vector<ctc::Station> stations_;
        std::vector<ctc::Train> train_schedules_;
        ctc::CtcOperationMode ctc_mode_;
        std::filesystem::path schedule_file_path_;
        Graph<types::BlockId, types::Meters> graph_;
};

std::vector<std::string> SplitBySemicolon(const std::string& input);

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H