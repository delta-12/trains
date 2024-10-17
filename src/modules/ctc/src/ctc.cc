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

/*------------------------------ Line Implementation ------------------------------*/
Line::Line(void)
{
}

Line::Line(const std::vector<types::Block> &block_list)
{
    for (const types::Block &block : block_list)
    {
        types::Block blk = block;
        block_map_[blk.block] = blk;
        if (blk.has_station)
        {
            ctc::Station station = Station(blk.station_name, blk.block);
            station_list_.push_back(station);
        }
    }
    // Implement Line (TrackId)
}

types::Block Line::GetBlock(const types::BlockId block_id)
{
    types::Block block;
    auto it = block_map_.find(block_id);
    if (it != block_map_.end())
    {
        block = it->second;
    }
    return block;
}

Station Line::GetStationByStationName(const std::string station_name)
{
    ctc::Station station;
    for (const auto &s : station_list_) {
        if (s.station_name == station_name) {
            station = s;
        }
    }
    return station;
}

/*------------------------------ Ctc Implementation ------------------------------*/
void Ctc::SetTrackLayout(const std::vector<types::Block> &blocks) 
{
    // Need Implementation
}

ctc::Line Ctc::GetLine(const types::TrackId track_id)
{
    auto it = track_layout_.find(track_id);
    if (it != track_layout_.end())
    {
        return it->second;
    }
    else 
    {
        throw std::runtime_error("ERROR: Track ID not found in Track Layout.");
    }
}

void Ctc::ManualDispatch(std::string &arrival_time, std::string &station_name, std::string &train_name, types::TrackId track_id) {
    auto it = std::find_if(train_schedule_.begin(), train_schedule_.end(),
                           [&](const auto &pair) {
                               return pair.second.train_name == train_name;
                           });
    ctc::Line line_info = GetLine(track_id);

    // Prepare DestinationAndArrivalTime object
    ctc::Station station = line_info.GetStationByStationName(station_name);
    std::string arrival_t = arrival_time;
    types::Tick t;
    DestinationAndArrivalTime dst(station, t);

    if (it != train_schedule_.end())
    {
        it->second.destination_list.push_back(dst);
    }
    else
    {
        Train new_train(train_name);
        new_train.destination_list.push_back(dst);
        // Implement Suggested Speed and Authority
        train_schedule_.insert({new_train.train_id, new_train});
    }
}

ctc::Train Ctc::GetTrainByName(const std::string train_name)
{
    ctc::Train train;
    for (const auto &t : train_schedule_) {
        if (t.second.train_name == train_name) {
            train = t.second;
        }
    }
    return train;
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC