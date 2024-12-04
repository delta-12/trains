/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "ctc.h"

#include <sstream>
#include "unordered_map"

namespace ctc
{

Ctc::Ctc(void) = default;

Ctc::Ctc(std::shared_ptr<TickSource> clk)
{
    clock_ = clk;
}

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

void Ctc::SetTrackLayout(void)
{
    CsvParser                 parser(schedule_file_path_);
    BlockBuilder              bb(parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetStations(blocks_);
    SetDefaultRoute();
}

void Ctc::SetTrackLayout(std::filesystem::path path)
{
    CsvParser                 parser(path);
    BlockBuilder              bb(parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetStations(blocks_);
    SetDefaultRoute();
}

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
    }
    return error;
}

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

void Ctc::AddTrainToTrainSchedule(ctc::Train train)
{
    train_schedules_.push_back(train);
}

types::Error Ctc::UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id)
{
    types::Error error = types::Error::ERROR_NONE;

    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );

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

types::Error Ctc::SetTrainDepartureTime(const std::string arrival_time, const types::Second seconds_to_travel_to_block, std::chrono::system_clock::time_point& departure_time)
{
    std::chrono::system_clock::time_point arrival_time_point;
    types::Error error = clock_->GetTimePoint(arrival_time, arrival_time_point);
    if (error == types::Error::ERROR_NONE)
    {
        std::chrono::seconds travel_time = std::chrono::duration_cast<std::chrono::seconds>(seconds_to_travel_to_block);
        departure_time = arrival_time_point - travel_time;
    }
    return error;
}

/*------------------------------------- Setters -------------------------------------*/
void Ctc::SetBlocks(std::vector<types::Block> &blocks)
{
    track_ = blocks[CTC_FIRST_BLOCK].track;
    // Emplace back Yard block
    blocks_.emplace_back();
    blocks_.back().block = 0;
    blocks_.back().track = track_;
    blocks_.insert(blocks_.end(), blocks.begin(), blocks.end());
}

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

void Ctc::SetScheduleFilePath(std::filesystem::path path)
{
    schedule_file_path_ = path;
}

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

void Ctc::SetManualMode(void)
{
    ctc_mode_ = CtcOperationMode::MANUAL_MODE;
}

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

void Ctc::SetSimulationSpeedMultiplier(int multiplier)
{
    clock_->SetMultiplier(static_cast<uint8_t>(multiplier));
}

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

/*------------------------------------- Getters -------------------------------------*/
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

std::size_t Ctc::GetNumStation(void) const
{
    return stations_.size();
}

std::vector<ctc::Station> Ctc::GetStations(void) const
{
    return stations_;
}

std::vector<types::BlockId> Ctc::GetDefaultRoute(void) const
{
    return default_route_;
}

std::vector<types::BlockId> Ctc::GetRoute(const types::BlockId destination)
{
    std::vector<types::BlockId> route;
    for (size_t i = 0; i < default_route_.size(); ++i)
    {
        if (default_route_[i] == destination)
        {
            std::copy(default_route_.begin(), default_route_.begin() + i + 1, std::back_inserter(route));
        }
    }
    return route;
}

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

ctc::CtcOperationMode Ctc::GetOperationMode(void) const
{
    return ctc_mode_;
}

types::TrackId Ctc::GetTrack(void) const
{
    return track_;
}

// std::vector<types::BlockId> Ctc::GetFailureBlocks(void) const
// {
//     return failure_blocks_;
// }

std::vector<types::Block> Ctc::GetBlocks(void) const
{
    return blocks_;
}

std::size_t Ctc::GetNumTrains(void) const
{
    return train_schedules_.size();
}

std::vector<ctc::Train> Ctc::GetTrains(void) const
{
    return train_schedules_;
}

std::vector<types::BlockId> Ctc::GetUpdatedBlocks(void) const
{
    return updated_blocks_;
}

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

std::string Ctc::GetTimeString(void) const
{
    return clock_->GetTimeString();
}

void Ctc::ClearUpdatedBlocks(void)
{
    updated_blocks_.clear();
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC