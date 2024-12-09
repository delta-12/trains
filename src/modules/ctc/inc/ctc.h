/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include <queue>

#include "block_builder.h"
#include "csv_parser.h"
#include "graph.h"
#include "types.h"
#include "file_explorer.h"
#include "tick_source.h"

#define CTC_YARD_BLOCK_0                  0
#define CTC_SECTION_D_BLOCK_13            13
#define CTC_SECTION_F_BLOCK_28            28
#define CTC_SECTION_J_BLOCK_58            58
#define CTC_SECTION_K_BLOCK_63            63
#define CTC_SECTION_M_BLOCK_76            76
#define CTC_SECTION_N_BLOCK_85            85
#define CTC_SECTION_R_BLOCK_101           101
#define CTC_FIRST_BLOCK                   0
#define CTC_TRAIN_CURRENT_DESTINATION     0
#define CTC_DEFAULT_DWELL_TIME_IN_SECONDS 300

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
    DestinationAndArrivalTime(void) : destination(0), arrival_time(std::chrono::system_clock::now())
    {
    };
    explicit DestinationAndArrivalTime(const types::BlockId destination) : destination(destination), arrival_time(std::chrono::system_clock::now())
    {
    };
    explicit DestinationAndArrivalTime(const types::BlockId destination, std::chrono::system_clock::time_point arrival_time) : destination(destination), arrival_time(arrival_time)
    {
    };
    types::BlockId destination;
    std::chrono::system_clock::time_point arrival_time;
};

struct Train
{
    Train(void) : train_id(GetNextId()), block_occupancy({}), current_position(0), suggested_speed(0), authority(), destination_list({}), departure_time(std::chrono::system_clock::now()), dwell_time(CTC_DEFAULT_DWELL_TIME_IN_SECONDS), dispatched(false)
    {
    };
    explicit Train(types::TrainId train_id) : train_id(train_id), block_occupancy({}), current_position(0), suggested_speed(0), authority(), destination_list({}), departure_time(std::chrono::system_clock::now()), dwell_time(CTC_DEFAULT_DWELL_TIME_IN_SECONDS), dispatched(false)
    {
    };
    types::TrainId train_id;
    std::vector<types::BlockId> block_occupancy;
    types::BlockId current_position;
    types::MetersPerSecond suggested_speed;
    std::queue<types::BlockId> authority;
    std::vector<DestinationAndArrivalTime> destination_list;
    std::chrono::system_clock::time_point departure_time;
    std::chrono::duration<double> dwell_time;
    bool dispatched;
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
        explicit Ctc(const types::TrackId track_id);
        Ctc(std::shared_ptr<TickSource> clk);
        Ctc(const types::TrackId track_id, std::shared_ptr<TickSource> clk);

        /* Integration */
        virtual types::Error SetBlockStates(const types::TrackId track, const std::vector<types::BlockState> &block_states);
        virtual std::vector<types::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const;

        /* Train Dispatch Specific */
        void SetSchedule(const std::vector<std::vector<std::string>> &records);   // Automatic Dispatch
        void ManualDispatch(types::TrainId train_id, types::BlockId destination); // Manual Dispatch to Block (not station)
        types::Error AutomaticDispatch(void);
        types::Error DispatchToStation(types::TrainId train_id, types::BlockId destination, std::string& arrival_time);
        types::Error UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id);
        types::Error ChooseFileAndSetTrackLayout(std::string &file_name);

        /* Setters */
        void SetTrackLayout(void);
        void SetTrackLayout(std::filesystem::path path);
        void SetScheduleFilePath(std::filesystem::path path);
        void SetManualMode(void);
        void SetBlockMaintenanceMode(const types::BlockId block_id, bool maintenance);
        void SetSimulationSpeedMultiplier(int multiplier);
        types::Error SetSwitchPosition(const types::BlockId block_id, const bool switched);
        types::Error SetTrainDepartureTime(const std::string arrival_time, const types::Second seconds_to_travel_to_block, std::chrono::system_clock::time_point& departure_time);
        void SetTrainDispatched(const types::TrainId train_id);

        /* Getters */
        types::Block GetBlockById(const types::BlockId block_id) const;
        std::vector<types::Block> GetBlocks(void) const;
        std::vector<ctc::Train> GetTrains(void) const;
        std::size_t GetNumStation(void) const;
        std::size_t GetNumTrains(void) const;
        std::vector<ctc::Station> GetStations(void) const;
        ctc::Station GetStationByName(const std::string& station_name);
        std::vector<types::BlockId> GetDefaultRoute(void) const;
        std::vector<types::BlockId> GetUpdatedBlocks(void) const;
        std::vector<ctc::Train> GetParsedSchedule(void) const;
        std::string GetTimeString(void) const;
        types::Tick GetTick(void) const;
        std::chrono::milliseconds GetTickDuration(void) const;
        types::Tick GetElapseTick(const types::Tick start, const types::Tick end) const;

        void ClearUpdatedBlocks(void);

        // Train Specific
        types::Error GetTrainById(const types::TrainId train_id, ctc::Train &train) const;
        std::size_t GetTrainAuthority(const types::TrainId train_id);
        types::MetersPerSecond GetTrainSuggestedSpeed(const types::TrainId train_id);
        types::BlockId GetTrainCurrentPosition(const types::TrainId train_id);
        ctc::DestinationAndArrivalTime GetTrainCurrentDestinationAndArrivalTime(const types::TrainId train_id);
        std::string GetTrainDepartureTime(const types::TrainId train_id);
        ctc::CtcOperationMode GetOperationMode(void) const;
        types::TrackId GetTrack(void) const;
        std::vector<types::BlockId> GetFailureBlocks(void) const;
        std::vector<types::BlockId> GetRoute(const types::BlockId start, const types::BlockId end);
        std::vector<types::BlockId> GetRoute(const types::BlockId destination);
        std::string TimePointToString(const std::chrono::system_clock::time_point& time_point);
        std::chrono::system_clock::time_point GetTime(void) const;

    private:
        void SetBlocks(std::vector<types::Block> &blocks);
        void SetStations(std::vector<types::Block> &blocks);
        void SetDefaultRoute(void);
        void AddTrainToTrainSchedule(ctc::Train train);
        void AssignAuthority(const std::vector<types::BlockId> &route, types::TrainId train_id);


        std::shared_ptr<TickSource> clock_;
        std::vector<types::Block> blocks_;
        std::vector<ctc::Station> stations_;
        std::vector<ctc::Train> train_schedules_;
        ctc::CtcOperationMode ctc_mode_;
        std::filesystem::path schedule_file_path_;
        Graph<types::BlockId, types::Meters> graph_;
        std::vector<types::BlockId> default_route_;
        types::TrackId track_;
        std::vector<types::BlockId> updated_blocks_;
        std::vector<ctc::Train> csv_schedules_;
};

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H