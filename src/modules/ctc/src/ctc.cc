/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "wayside_controller_gateway.h"
#include "ctc.h"

#include <sstream>
#include "unordered_map"
#include <iostream>


namespace ctc
{

void Ctc::SetTrackLayout(void)
{
    CsvParser                 parser(schedule_file_path_);
    BlockBuilder              bb(parser.GetRecords(), Module::MODULE_CTC);
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
    types::Error error = types::ERROR_NONE;
    ctc::Train*  train = GetTrainPointerById(train_id);
    if (train != nullptr)
    {
        train->authority.pop();
        types::BlockId current_block_id = train->authority.front();
        types::Block   current_block    = GetBlockById(current_block_id);
        train->suggested_speed = current_block.speed_limit;
    }
    else
    {
        error = types::ERROR_INVALID_TRAIN;
    }
    return error;
}

/*------------------------------------- Setters -------------------------------------*/
void Ctc::SetBlocks(std::vector<types::Block> &blocks)
{
    types::Block yard;
    yard.block = 0;
    blocks_.push_back(yard);
    for ( types::Block &block : blocks)
    {
        blocks_.push_back(block);
    }
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
    default_route_.push_back(yard);
    // Add K, L, M, N, O, P, Q
    for (std::size_t i = 63; i < 101; ++i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    // Add N
    for (std::size_t i = 85; i > 76; --i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = 101; i < blocks_.size(); ++i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = 28; i > 0; --i)
    {
        default_route_.push_back(blocks_[i].block);
    }
    for (std::size_t i = 13; i < 58; ++i)
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

ctc::Train* Ctc::GetTrainPointerById(const types::TrainId train_id)
{
    ctc::Train* train_pointer;
    for (ctc::Train &train : train_schedules_)
    {
        if (train.train_id == train_id)
        {
            train_pointer = &train;
        }
    }
    return train_pointer;
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

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC