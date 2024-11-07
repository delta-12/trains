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

void Ctc::SetTrackLayout(void)
{
    CsvParser                 parser(schedule_file_path_);
    BlockBuilder              bb(parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetStations(blocks_);
    SetDefaultRoute();
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

void Ctc::ManualDispatch(types::BlockId destination)
{
    ctc::Train train;
    AddTrainToTrainSchedule(train);
    std::vector<types::BlockId> route = GetRoute(destination);
    AssignAuthority(route, train.train_id);
}

void Ctc::AddTrainToTrainSchedule(ctc::Train train)
{
    train_schedules_.push_back(train);
}

types::Error Ctc::UpdateSuggestedSpeedAndAuthority(const types::TrainId train_id)
{
    types::Error error = types::Error::ERROR_INVALID_TRAIN;

    std::vector<ctc::Train>::iterator train_it = std::find_if(
        train_schedules_.begin(),
        train_schedules_.end(),
        [train_id](const ctc::Train &train) {
            return train.train_id == train_id;
        }
        );

    if (train_it != train_schedules_.end())
    {
        train_it->authority.pop();
        types::BlockId current_block_id = train_it->authority.front();
        types::Block   current_block    = GetBlockById(current_block_id);
        train_it->suggested_speed = current_block.speed_limit;
        error                     = types::Error::ERROR_NONE;
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
            // Update block states in private data memer blocks_ which stores all blocks information
            std::vector<types::Block>::iterator block_it = std::find_if(
                blocks_.begin(),
                blocks_.end(),
                [block_state](const types::Block &block) {
                    return block.block == block_state.block;
                }
                );

            if (block_it != blocks_.end())
            {
                block_it->occupied = block_state.occupied;
                if (block_state.track_failure == true)
                {
                    failure_blocks_.push_back(block_state.block);
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
                error = types::Error::ERROR_NONE;
            }
            else
            {
                error = types::Error::ERROR_INVALID_TRAIN;
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

/*------------------------------------- Setters -------------------------------------*/
void Ctc::SetBlocks(std::vector<types::Block> &blocks)
{
    types::Block yard;
    yard.block = 0;
    blocks_.push_back(yard);
    for (types::Block &block : blocks)
    {
        blocks_.push_back(block);
    }
    track_ = blocks[CTC_FIRST_BLOCK].track;
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

ctc::Train Ctc::GetTrainById(const types::TrainId train_id) const
{
    ctc::Train result;
    for (ctc::Train train : train_schedules_)
    {
        if (train.train_id == train_id)
        {
            result = train;
        }
    }
    return result;
}

ctc::CtcOperationMode Ctc::GetOperationMode(void) const
{
    return ctc_mode_;
}

types::TrackId Ctc::GetTrack(void) const
{
    return track_;
}

std::vector<types::BlockId> Ctc::GetFailureBlocks(void) const
{
    return failure_blocks_;
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC