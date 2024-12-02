/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include <queue>

#include "types.h"
#include "block_builder.h"
#include "csv_parser.h"
#include "graph.h"
#include "file_explorer.h"
#include "tick_source.h"

#define CTC_YARD_BLOCK_0              0
#define CTC_SECTION_D_BLOCK_13        13
#define CTC_SECTION_F_BLOCK_28        28
#define CTC_SECTION_J_BLOCK_58        58
#define CTC_SECTION_K_BLOCK_63        63
#define CTC_SECTION_M_BLOCK_76        76
#define CTC_SECTION_N_BLOCK_85        85
#define CTC_SECTION_R_BLOCK_101       101
#define CTC_FIRST_BLOCK               0
#define CTC_TRAIN_CURRENT_DESTINATION 0

namespace ctc
{

typedef uint16_t                         column;
typedef struct DestinationAndArrivalTime DestinationAndArrivalTime;
typedef struct Train                     Train;
typedef struct Station                   Station;

typedef enum
{
    AUTOMATIC_MODE,
    MANUAL_MODE,
    MAINTENANCE_MODE
} CtcOperationMode;

struct Line
{
    Line(void) : track(types::TrackId::TRACKID_NONE)
    {
    };
    types::TrackId track;
    std::vector<types::Block> blocks;
    std::vector<ctc::Station> stations;
    std::vector<ctc::Train> train_schedules;
    std::filesystem::path schedule_file_path;
    Graph<types::BlockId, types::Meters> graph;
    std::vector<types::BlockId> default_route;
    std::vector<types::BlockId> failure_blocks;
    std::vector<types::BlockId> updated_blocks;
};

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
    DestinationAndArrivalTime(void) : destination(0), arrival_time(0)
    {
    };
    explicit DestinationAndArrivalTime(const types::BlockId destination) : destination(destination), arrival_time(0)
    {
    };
    explicit DestinationAndArrivalTime(const types::BlockId destination, types::Tick arrival_time) : destination(destination), arrival_time(arrival_time)
    {
    };
    types::BlockId destination;
    types::Tick arrival_time;
};

struct Train
{
    Train(void) : train_id(GetNextId()), train_name(""), block_occupancy({}), current_position(0), suggested_speed(0), authority(), destination_list({})
    {
    };
    explicit Train(types::TrainId train_id) : train_id(train_id), block_occupancy({}), current_position(0), suggested_speed(0), authority(), destination_list({})
    {
    };
    Train(std::string train_name) : train_name(train_name), block_occupancy({}), current_position(0), suggested_speed(0), authority(), destination_list({})
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
        /* Constructor */
        Ctc(void);
        Ctc(std::shared_ptr<TickSource> clk);
        explicit Ctc(const types::TrackId track_id);

        /* Integration */
        types::Error SetBlockStates(const types::TrackId track, const std::vector<types::BlockState> &block_states);
        std::vector<types::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const;

        /* Train Dispatch Specific */
        void SetSchedule(const types::TrainId train, const std::vector<DestinationAndArrivalTime> &schedule); // Automatic Dispatch
        void ManualDispatch(types::TrainId train_id, types::BlockId destination);                             // Manual Dispatch to Block (not station)
        types::Error UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id);
        types::Error ChooseFileAndSetTrackLayout(std::string &file_name);

        /* Setters */
        void SetTrackLayout(void);
        void SetTrackLayout(std::filesystem::path path);
        void SetScheduleFilePath(std::filesystem::path path);
        void SetManualMode(void);
        void SetBlockToMaintenance(types::BlockId block_id);
        void SetBlockToOpen(types::BlockId block_id);

        /* Getters */
        types::Block GetBlockById(const types::BlockId block_id) const;
        std::vector<types::Block> GetBlocks(void) const;
        std::vector<ctc::Train> GetTrains(void) const;
        std::size_t GetNumStation(void) const;
        std::size_t GetNumTrains(void) const;
        std::vector<ctc::Station> GetStations(void) const;
        std::vector<types::BlockId> GetDefaultRoute(void) const;
        std::vector<types::BlockId> GetUpdatedBlocks(void) const;
        std::string GetTimeString(void) const;
        void ClearUpdatedBlocks(void);

        // Train Specific
        types::Error GetTrainById(const types::TrainId train_id, ctc::Train &train) const;
        std::size_t GetTrainAuthority(const types::TrainId train_id);
        types::MetersPerSecond GetTrainSuggestedSpeed(const types::TrainId train_id);
        types::BlockId GetTrainCurrentPosition(const types::TrainId train_id);
        ctc::CtcOperationMode GetOperationMode(void) const;
        types::TrackId GetTrack(void) const;
        std::vector<types::BlockId> GetFailureBlocks(void) const;
        std::vector<types::BlockId> GetRoute(const types::BlockId start, const types::BlockId end);

    private:
        void SetBlocks(std::vector<types::Block> &blocks);
        void SetStations(std::vector<types::Block> &blocks);
        void SetDefaultRoute(void);
        void AddTrainToTrainSchedule(ctc::Train train);
        void AssignAuthority(const std::vector<types::BlockId> &route, types::TrainId train_id);
        std::vector<types::BlockId> GetRoute(const types::BlockId destination);

        std::shared_ptr<TickSource> clock_;
        std::vector<types::Block> blocks_;
        std::vector<ctc::Station> stations_;
        std::vector<ctc::Train> train_schedules_;
        ctc::CtcOperationMode ctc_mode_;
        std::filesystem::path schedule_file_path_;
        Graph<types::BlockId, types::Meters> graph_;
        std::vector<types::BlockId> default_route_;
        types::TrackId track_;
        std::vector<types::BlockId> failure_blocks_;
        std::vector<types::BlockId> updated_blocks_;
};

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H