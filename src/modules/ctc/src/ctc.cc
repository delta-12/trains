/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "ctc.h"

#include <sstream>
#include <iostream>
#include "unordered_map"

namespace ctc
{

/* Default Constructor*/
Ctc::Ctc(void) = default;

/**
 * CTC constructor
 *
 * @param clk shared pointer to tick source.
 */
Ctc::Ctc(std::shared_ptr<TickSource> clk)
{
    clock_ = clk;
}

/**
 * CTC constructor that loads green line by default without having to choose a csv file
 *
 * @param track_id enum class indicates line color (green or red or blue). See types.h for more details
 */
Ctc::Ctc(const types::TrackId track_id)
{
    if (track_id == types::TrackId::TRACKID_GREEN)
    {
        std::filesystem::path base_path = std::filesystem::current_path();
        std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
        // Check if path exist
        if (std::filesystem::exists(path))
        {
            SetScheduleFilePath(path);
            SetTrackLayout();
        }
    }
}

/**
 * CTC constructor that loads green line schedule by default and initialized with a shared pointer to tick source
 *
 * @param track_id enum class indicates line color (green or red or blue). See types.h for more details
 * @param clk shared pointer to tick source.
 */
Ctc::Ctc(const types::TrackId track_id, std::shared_ptr<TickSource> clk)
{
    clock_ = clk;
    if (track_id == types::TrackId::TRACKID_GREEN)
    {
        std::filesystem::path base_path = std::filesystem::current_path();
        std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
        // Check if path exist
        if (std::filesystem::exists(path))
        {
            SetScheduleFilePath(path);
            SetTrackLayout();
        }
    }
}

/**
 * Populate private data member blocks_ with vectors of blocks parsed from csv file
 */
void Ctc::SetTrackLayout(void)
{
    CsvParser                 parser(schedule_file_path_);
    BlockBuilder              bb(parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetStations(blocks_);
    SetSchedule(parser.GetRecords());
    SetDefaultRoute();
}

/**
 * Populate private data member blocks_ with vectors of blocks parsed input file path
 *
 * @param path filesystem path to chosen csv file
 */
void Ctc::SetTrackLayout(std::filesystem::path path)
{
    CsvParser                 parser(path);
    BlockBuilder              bb(parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetStations(blocks_);
    SetSchedule(parser.GetRecords());
    SetDefaultRoute();
}

/**
 * Populate private data member csv_schedules_ with vectors of trains struct parsed from vector of records
 *
 * @param records vectors of vectors of strings which is parsed from a csv file
 */
void Ctc::SetSchedule(const std::vector<std::vector<std::string>> &records)
{
    // Get first row of the csv file
    std::vector<std::string> first_record = records[0];
    for (size_t i = 0; i < first_record.size(); ++i)
    {
        // Find the key word "Train"
        std::string input   = first_record[i];
        std::string keyword = "Train";
        size_t      pos     = input.find(keyword);
        if (pos != std::string::npos)
        {
            pos += keyword.length();

            // Skip any spaces after "Train" to get train number
            while (pos < input.length() && std::isspace(input[pos]))
            {
                pos++;
            }
            int trainNumber = 0;
            while (pos < input.length() && std::isdigit(input[pos]))
            {
                trainNumber = trainNumber * 10 + (input[pos] - '0');
                pos++;
            }
            if (trainNumber > 0)
            {
                ctc::Train train(trainNumber);
                // Iterate through all rows
                for (size_t j = 1; j < records.size(); ++j)
                {
                    // Retreive destination station and arrival time for current train number
                    if (!records[j][i].empty() && GetBlockById(std::stoi(records[j][2])).has_station)
                    {
                        std::chrono::system_clock::time_point arrival_time_point;
                        clock_->GetTimePoint(records[j][i], arrival_time_point);
                        train.destination_list.emplace_back(std::stoi(records[j][2]), arrival_time_point);
                    }
                }
                csv_schedules_.push_back(train);
            }
        }
    }
}

/**
 * Open File Explorer pop-up for user to select a csv file
 *
 * @param [out] file_name variable that store selected csv file name
 * @return Error code from types.h
 */
types::Error Ctc::ChooseFileAndSetTrackLayout(std::string &file_name)
{
    types::Error          error = types::Error::ERROR_NONE;
    FileExplorer          file_explorer;
    std::filesystem::path path = file_explorer.GetPath();
    file_name = file_explorer.GetFileName();
    if (path.empty() | file_name.empty())
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        SetTrackLayout(path);
        SetScheduleFilePath(path);
    }
    return error;
}

/**
 * Convert route, which is a vector of block IDs, into an authority queue inside train struct
 *
 * @param route vector of block IDs that train needs to travel to get from one point to another.
 * @param train_id id for train which authority needs to be assigned.
 */
void Ctc::AssignAuthority(const std::vector<types::BlockId> &route, types::TrainId train_id)
{
    std::queue<types::BlockId> authority(std::deque<types::BlockId>(route.begin(), route.end()));

    for (ctc::Train &train : train_schedules_)
    {
        if (train.train_id == train_id)
        {
            train.authority = authority;
        }
    }
}

/**
 * Manually dispatch a new train or add a destination station to an already existing train
 *
 * @param train_id id for train.
 * @param destination block ID that trains need to travel to.
 */
void Ctc::ManualDispatch(types::TrainId train_id, types::BlockId destination)
{
    auto train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        train_it->destination_list.emplace_back(DestinationAndArrivalTime(destination));
    }
    else
    {
        ctc::Train train(train_id);
        train.destination_list.emplace_back(DestinationAndArrivalTime(destination));
        AddTrainToTrainSchedule(train);
        std::vector<types::BlockId> route = GetRoute(destination);
        AssignAuthority(route, train.train_id);
    }
}

/**
 * Manually dispatch a new train or add a destination station to an already existing train
 *
 * @param train_id id for train.
 * @param destination block ID that trains need to travel to.
 * @param arrival_time train desired arrival time to destination.
 */
types::Error Ctc::DispatchToStation(types::TrainId train_id, types::BlockId destination, std::string& arrival_time)
{
    types::Error error    = types::Error::ERROR_NONE;
    auto         train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        train_it->destination_list.emplace_back(DestinationAndArrivalTime(destination));
    }
    else
    {
        ctc::Train                            train(train_id);
        std::chrono::system_clock::time_point arrival_time_point;
        clock_->GetTimePoint(arrival_time, arrival_time_point);
        train.destination_list.emplace_back(DestinationAndArrivalTime(destination, arrival_time_point));
        error = SetTrainDepartureTime(arrival_time, GetBlockById(destination).total_time_to_station, train.departure_time);
        if (error == types::Error::ERROR_NONE)
        {
            AddTrainToTrainSchedule(train);
            std::vector<types::BlockId> route = GetRoute(destination);
            AssignAuthority(route, train.train_id);
        }
    }
    return error;
}

/**
 * Automatically dispatch trains in privated member csv_schedule_
 *
 * @return Error code from types.h
 */
types::Error Ctc::AutomaticDispatch(void)
{
    types::Error error = types::Error::ERROR_NONE;
    for (ctc::Train train : csv_schedules_)
    {
        std::string    arrival_time = TimePointToString(train.destination_list[0].arrival_time);
        types::BlockId destination  = train.destination_list[0].destination;
        error = SetTrainDepartureTime(arrival_time, GetBlockById(destination).total_time_to_station, train.departure_time);
        if (error != types::Error::ERROR_NONE)
        {
            break;
        }
        else
        {
            AddTrainToTrainSchedule(train);
            std::vector<types::BlockId> route = GetRoute(destination);
            AssignAuthority(route, train.train_id);
        }
    }
    return error;
}

/**
 * Add train object to private data member train_schedules_. Any train being added to this data member is considered prepare for dispatch
 *
 * @param train train struct
 */
void Ctc::AddTrainToTrainSchedule(ctc::Train train)
{
    train_schedules_.push_back(train);
}

/**
 * Update train suggeted speed and authority based on train_id input
 *
 * @param train_id id for train.
 * @return Error code from types.h.
 */
types::Error Ctc::UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id)
{
    types::Error error = types::Error::ERROR_NONE;

    // Find train object that matches with train_id within train_schedules
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    // If train object exist, update next destination or pop authority depending if train has reach its current destination
    if (train_it != train_schedules_.end())
    {
        // If train reach current destination
        if (train_it->current_position == train_it->destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination)
        {
            // If no destination left
            if (train_it->destination_list.size() <= 1)
            {
                train_it->destination_list.clear();
                train_it->destination_list.emplace_back(DestinationAndArrivalTime(CTC_YARD_BLOCK_0));
                std::vector<types::BlockId> route = GetRoute(train_it->current_position, CTC_YARD_BLOCK_0);
                AssignAuthority(route, train_it->train_id);
            }
            // Else Assign Route to next destination
            else
            {
                train_it->destination_list.erase(train_it->destination_list.begin());
                std::vector<types::BlockId> route = GetRoute(train_it->current_position, train_it->destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination);
                AssignAuthority(route, train_it->train_id);
            }
        }
        // Else pop authority queue
        else
        {
            train_it->authority.pop();
            types::BlockId current_block_id = train_it->authority.front();
            types::Block   current_block    = GetBlockById(current_block_id);
            train_it->suggested_speed = current_block.speed_limit;
        }
    }
    else
    {
        error = types::Error::ERROR_INVALID_TRAIN;
    }
    return error;
}

/**
 * API for Wayside Controller to send occupancy and failure signal
 * If occupied signal block_states contains a block_id that is at the top of authority of a train object, it means
 * that specific train has move from the previous block to this new block. Therefore, update train suggested speed, authority and current position
 *
 * @param block_states block state struct which contains block_id, occupied boolean and failure boolean. See definition in wayside_controller.h.
 * @param track track id. Refer types.h.
 * @return Error code from types.h.
 */
types::Error Ctc::SetBlockStates(const types::TrackId track, const std::vector<types::BlockState> &block_states)
{
    types::Error   error     = types::Error::ERROR_NONE;
    types::TrackId ctc_track = GetTrack();
    if (track == ctc_track)
    {
        for (const types::BlockState &block_state : block_states)
        {
            // Push block ID back to updated_blocks_
            updated_blocks_.emplace_back(block_state.block);

            // Update block states in private data memer blocks_ which stores all blocks information
            std::vector<types::Block>::iterator block_it = std::find_if(blocks_.begin(), blocks_.end(), [block_state](const types::Block &block) {
                    return block.block == block_state.block;
                });

            if (block_it != blocks_.end())
            {
                if (block_state.track_failure == true)
                {
                    block_it->failed = block_state.track_failure;
                }
                else
                {
                    block_it->occupied = block_state.occupied;
                }
            }
            else
            {
                error = types::Error::ERROR_INVALID_BLOCK;
            }

            // Update Train current position, suggested speed and authority upon receiving block occupancy
            std::vector<ctc::Train>::iterator train_it = std::find_if(
                train_schedules_.begin(),
                train_schedules_.end(),
                [block_state](ctc::Train &train) {
                    return !train.authority.empty() && train.authority.front() == block_state.block;
                }
                );
            if (train_it != train_schedules_.end())
            {
                train_it->current_position = block_state.block;
                UpdateSuggestedSpeedAndAuthority(train_it->train_id);
            }
        }
    }
    return error;
}

/**
 * Get suggested speeds and authority for all active trains
 *
 * @return vector of TrackCircuitData structs.
 */
std::vector<types::TrackCircuitData> Ctc::GetSuggestedSpeedsAndAuthorities(void) const
{
    std::vector<types::TrackCircuitData> suggested_speed_and_authorities;
    for (const ctc::Train &train : train_schedules_)
    {
        suggested_speed_and_authorities.emplace_back(
            GetBlockById(train.current_position).track,
            train.current_position,
            train.suggested_speed,
            train.authority.size()
            );
    }
    return suggested_speed_and_authorities;
}

/**
 * Calculate train's departure time
 *
 * @param [in] arrival_time train's arrival time to its current destination
 * @param [in] seconds_to_travel_to_block how long it takes for train to travel to a specific station
 * @param [out] departure_time time point for train to be dispatched at
 * @return Error code from types.h.
 */
types::Error Ctc::SetTrainDepartureTime(const std::string arrival_time, const types::Second seconds_to_travel_to_block, std::chrono::system_clock::time_point& departure_time)
{
    std::chrono::system_clock::time_point arrival_time_point;
    types::Error                          error = clock_->GetTimePoint(arrival_time, arrival_time_point);
    if (error == types::Error::ERROR_NONE)
    {
        std::chrono::seconds travel_time = std::chrono::duration_cast<std::chrono::seconds>(seconds_to_travel_to_block);
        departure_time = arrival_time_point - travel_time;
    }
    return error;
}

/*------------------------------------- Setters -------------------------------------*/
/**
 * Populate CTC private data member blocks_ from a vector of blocks struct
 *
 * @param blocks vector of blocks struct containing blocks data parsed from csv schedule
 */
void Ctc::SetBlocks(std::vector<types::Block> &blocks)
{
    track_ = blocks[CTC_FIRST_BLOCK].track;
    // Emplace back Yard block
    blocks_.emplace_back();
    blocks_.back().block = 0;
    blocks_.back().track = track_;
    blocks_.insert(blocks_.end(), blocks.begin(), blocks.end());
}

/**
 * Populate CTC private data member stations_ from a vector of blocks struct
 *
 * @param blocks vector of blocks struct containing blocks data parsed from csv schedule
 */
void Ctc::SetStations(std::vector<types::Block> &blocks)
{
    for (types::Block block : blocks)
    {
        if (block.has_station)
        {
            ctc::Station station(block.station_name, block.block, block.total_time_to_station);
            stations_.push_back(station);
        }
    }
}

/**
 * Assign a file path to CTC private data member schedule_file_path_
 *
 * @param path path to selected csv file
 */
void Ctc::SetScheduleFilePath(std::filesystem::path path)
{
    schedule_file_path_ = path;
}

/**
 * Create a default route for green line
 */
void Ctc::SetDefaultRoute(void)
{
    types::BlockId yard = 0;
    // Add K, L, M, N, O, P, Q
    for (std::size_t i = CTC_SECTION_K_BLOCK_63; i < CTC_SECTION_R_BLOCK_101; ++i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    // Add N
    for (std::size_t i = CTC_SECTION_N_BLOCK_85; i > CTC_SECTION_M_BLOCK_76; --i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = CTC_SECTION_R_BLOCK_101; i < blocks_.size(); ++i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = CTC_SECTION_F_BLOCK_28; i > CTC_YARD_BLOCK_0; --i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = CTC_SECTION_D_BLOCK_13; i < CTC_SECTION_J_BLOCK_58; ++i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    default_route_.push_back(yard);
}

/**
 * Set CTC private data member ctc_mode_ to manual mode
 */
void Ctc::SetManualMode(void)
{
    ctc_mode_ = CtcOperationMode::MANUAL_MODE;
}

/**
 * Assign a file path to CTC private data member schedule_file_path_
 *
 * @param path path to selected csv file
 */
void Ctc::SetBlockMaintenanceMode(const types::BlockId block_id, bool maintenance)
{
    std::vector<types::Block>::iterator block_it = std::find_if(blocks_.begin(), blocks_.end(), [block_id](const types::Block &block) {
            return block.block == block_id;
        });

    if (block_it != blocks_.end())
    {
        if (maintenance == true)
        {
            block_it->maintenance = true;
        }
        else
        {
            block_it->maintenance = false;
            block_it->failed      = false;
        }

    }
}

/**
 * Increase simulation speed
 *
 * @param multiplier simulation speed multiplier value
 */
void Ctc::SetSimulationSpeedMultiplier(int multiplier)
{
    clock_->SetMultiplier(static_cast<uint8_t>(multiplier));
}

/**
 * Set switch position for a block
 *
 * @param block_id ID for a block. Refer types.h.
 * @param track track id. Refer types.h.
 * @return Error code from types.h.
 */
types::Error Ctc::SetSwitchPosition(const types::BlockId block_id, const bool switched)
{
    types::Error                        error    = types::Error::ERROR_NONE;
    std::vector<types::Block>::iterator block_it = std::find_if(blocks_.begin(), blocks_.end(), [block_id](const types::Block &block) {
            return block.block == block_id;
        });

    if (block_it != blocks_.end())
    {
        block_it->switched = switched;
    }
    else
    {
        error = types::Error::ERROR_INVALID_BLOCK;
    }
    return error;
}

/**
 * Mark that train has been dispatched
 *
 * @param train_id ID for a train. Refer types.h.
 */
void Ctc::SetTrainDispatched(const types::TrainId train_id)
{
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        train_it->dispatched = true;
    }
}

/*------------------------------------- Getters -------------------------------------*/
/**
 * Return block struct by ID
 *
 * @param block_id ID for a block. Refer types.h.
 */
types::Block Ctc::GetBlockById(const types::BlockId block_id) const
{
    types::Block result;
    for (types::Block block : blocks_)
    {
        if (block.block == block_id)
        {
            result = block;
        }
    }
    return result;
}

/**
 * Return number of stations in CTC class
 */
std::size_t Ctc::GetNumStation(void) const
{
    return stations_.size();
}

/**
 * Return a vetor of station structs in CTC class
 */
std::vector<ctc::Station> Ctc::GetStations(void) const
{
    return stations_;
}

/**
 * Return CTC green line default route
 */
std::vector<types::BlockId> Ctc::GetDefaultRoute(void) const
{
    return default_route_;
}

/**
 * Find the route a train needs to travel to get to a specific block
 *
 * @param destination block ID of the destination
 * @return vector of block IDs
 */
std::vector<types::BlockId> Ctc::GetRoute(const types::BlockId destination)
{
    std::vector<types::BlockId> route;
    // Find the first occurrence of the destination block
    auto it = std::find(default_route_.begin(), default_route_.end(), destination);

    // If the destination block is found, copy the portion of the route
    if (it != default_route_.end())
    {
        std::copy(default_route_.begin(), it + 1, std::back_inserter(route));
    }

    return route;
}

/**
 * Find the route a train needs to travel from one block to another block
 *
 * @param start ID of the starting block
 * @param destination block ID of the destination
 * @return vector of block IDs
 */
std::vector<types::BlockId> Ctc::GetRoute(const types::BlockId start, const types::BlockId end)
{
    std::vector<types::BlockId> route;

    std::vector<types::BlockId>::iterator start_itr = std::find(default_route_.begin(), default_route_.end(), start);
    std::vector<types::BlockId>::iterator end_itr   = std::find(default_route_.begin(), default_route_.end(), end);

    if (start_itr != default_route_.end() && end_itr != default_route_.end() && start_itr < end_itr)
    {
        // Return the subvector from the element after start to the element end
        route = std::vector<types::BlockId>(start_itr + 1, end_itr + 1);
    }
    return route;
}

/**
 * Find a specific train struct within train_schedules_
 *
 * @param [in] train_id ID of the train that needs to be returned
 * @param [out] train train struct
 * @return Error code in types.h.
 */
types::Error Ctc::GetTrainById(const types::TrainId train_id, ctc::Train &train) const
{
    types::Error error = types::Error::ERROR_NONE;
    for (const ctc::Train &t : train_schedules_)
    {
        if (t.train_id == train_id)
        {
            train = t;
        }
        else
        {
            error = types::Error::ERROR_INVALID_TRAIN;
        }
    }
    return error;
}

/**
 * Get CTC current operation mode
 *
 * @return CTC operation mode
 */
ctc::CtcOperationMode Ctc::GetOperationMode(void) const
{
    return ctc_mode_;
}

/**
 * Get CTC track ID
 *
 * @return CTC track ID
 */
types::TrackId Ctc::GetTrack(void) const
{
    return track_;
}

/**
 * Get CTC list of blocks
 *
 * @return vector of block structs
 */
std::vector<types::Block> Ctc::GetBlocks(void) const
{
    return blocks_;
}

/**
 * Get CTC number of trains
 *
 * @return number of trains
 */
std::size_t Ctc::GetNumTrains(void) const
{
    return train_schedules_.size();
}

/**
 * Get all active train(dispatched or in yard) within the CTC
 *
 * @return vector of train structs
 */
std::vector<ctc::Train> Ctc::GetTrains(void) const
{
    return train_schedules_;
}

/**
 * Get updated blocks
 *
 * @return vector of block IDs
 */
std::vector<types::BlockId> Ctc::GetUpdatedBlocks(void) const
{
    return updated_blocks_;
}

/**
 * Get CTC current tick source
 *
 * @return integer value. Refer to types.h
 */
types::Tick Ctc::GetTick(void) const
{
    return clock_->GetTick();
}

/**
 * Get CTC current tick source's tick duration
 *
 * @return millisecond value
 */
std::chrono::milliseconds Ctc::GetTickDuration(void) const
{
    return clock_->GetTickDuration();
}

/**
 * Get CTC tick source elapse tick
 *
 * @param start starting tick
 * @param end ending tick
 * @return integer value. Refer to types.h
 */
types::Tick Ctc::GetElapseTick(const types::Tick start, const types::Tick end) const
{
    return clock_->GetElapsedTicks(start, end);
}

/**
 * Get a specific train's authority
 *
 * @param train_id ID for a train.
 * @return number of blocks train has to travel
 */
std::size_t Ctc::GetTrainAuthority(const types::TrainId train_id)
{
    std::size_t                       authority;
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        authority = train_it->authority.size();
    }
    else
    {
        authority = 0;
    }
    return authority;
}

/**
 * Get a specific train's suggested speed
 *
 * @param train_id ID for a train.
 * @return suggested speed
 */
types::MetersPerSecond Ctc::GetTrainSuggestedSpeed(const types::TrainId train_id)
{
    types::MetersPerSecond            suggested_speed;
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        suggested_speed = train_it->suggested_speed;
    }
    else
    {
        suggested_speed = 0;
    }
    return suggested_speed;
}

/**
 * Get a specific train's current position
 *
 * @param train_id ID for a train.
 * @return Block ID
 */
types::BlockId Ctc::GetTrainCurrentPosition(const types::TrainId train_id)
{
    {
        types::BlockId                    current_position;
        std::vector<ctc::Train>::iterator train_it = std::find_if(
            train_schedules_.begin(),
            train_schedules_.end(),
            [train_id](const ctc::Train &train) {
                return train.train_id == train_id;
            }
            );
        if (train_it != train_schedules_.end())
        {
            current_position = train_it->current_position;
        }
        else
        {
            current_position = 0;
        }
        return current_position;
    }
}

/**
 * Get a specific train's current destination and arrival time
 *
 * @param train_id ID for a train.
 * @return DestinationAndArrivalTime struct
 */
ctc::DestinationAndArrivalTime Ctc::GetTrainCurrentDestinationAndArrivalTime(const types::TrainId train_id)
{
    ctc::DestinationAndArrivalTime    destination_and_arrival_time;
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        destination_and_arrival_time = train_it->destination_list[CTC_TRAIN_CURRENT_DESTINATION];
    }
    return destination_and_arrival_time;
}

/**
 * Get a specific train's departure time
 *
 * @param train_id ID for a train.
 * @return string of departure time
 */
std::string Ctc::GetTrainDepartureTime(const types::TrainId train_id)
{
    std::string                       departure_time;
    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );
    if (train_it != train_schedules_.end())
    {
        std::chrono::system_clock::time_point departure_time_point = train_it->departure_time;
        departure_time = TimePointToString(departure_time_point);
    }
    return departure_time;
}

/**
 * Get CTC list of trains object parsed from csv schedule. These trains are not yet dispatched
 *
 * @return vector of train structs
 */
std::vector<ctc::Train> Ctc::GetParsedSchedule(void) const
{
    return csv_schedules_;
}

/**
 * Get CTC tick source's current time
 *
 * @return string of current time
 */
std::string Ctc::GetTimeString(void) const
{
    return clock_->GetTimeString();
}

/**
 * Get CTC tick source's current time
 *
 * @return current time in time point
 */
std::chrono::system_clock::time_point Ctc::GetTime(void) const
{
    return clock_->GetTime();
}

/**
 * Get Station struct by station name
 *
 * @param station_name string of a station's name
 * @return station struct
 */
ctc::Station Ctc::GetStationByName(const std::string& station_name)
{
    ctc::Station result;
    for (ctc::Station station : stations_)
    {
        if (station.station_name == station_name)
        {
            result = station;
        }
    }
    return result;
}

/**
 * Erase all element within CTC private data member updated_blocks_. This is because callback_handler is checking if there is any updated blocks, if there is, then update UI.
 * After update UI, clear updated blocks.
 */
void Ctc::ClearUpdatedBlocks(void)
{
    updated_blocks_.clear();
}

/**
 * Convert system clock time point to a string
 *
 * @param time_point time point object
 * @return string version of the time point
 */
std::string Ctc::TimePointToString(const std::chrono::system_clock::time_point& time_point)
{
    std::stringstream buffer;
    std::time_t       time_t_point = std::chrono::system_clock::to_time_t(time_point);
    std::tm           local_time   = *std::localtime(&time_t_point);
    buffer << std::put_time(&local_time, "%T");
    return buffer.str();
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC