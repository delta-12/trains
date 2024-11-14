#include "simulator.h"

#include <algorithm>

namespace simulator
{

types::Error Simulator::AddTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    types::Error   error    = types::Error::ERROR_NONE;
    types::TrackId track_id = track.get()->GetTrackId();

    if (tracks_.contains(track_id))
    {
        error = types::Error::ERROR_DUPLICATE_TRACK;
    }
    else
    {
        tracks_[track_id] = track;
    }

    return error;
}

types::Error Simulator::DeleteTrackModel(const types::TrackId track)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        tracks_.erase(track);
    }

    return error;
}

void Simulator::GetTrackModels(std::vector<std::shared_ptr<track_model::TrackModel>> &tracks) const
{
    tracks.reserve(tracks_.size() - tracks.size());

    for (const std::pair<types::TrackId, std::shared_ptr<track_model::TrackModel>> track_model : tracks_)
    {
        tracks.emplace_back(track_model.second);
    }
}

types::Error Simulator::AddTrainModel(const types::TrackId track, std::shared_ptr<train_model::TrainModel> train)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        tracks_[track].get()->AddTrainModel(train);
    }

    return error;
}

std::shared_ptr<train_model::TrainModel> Simulator::GetTrainModel(const types::TrainId train)
{
    std::shared_ptr<train_model::TrainModel> model(nullptr);

    for (const std::pair<types::TrackId, std::shared_ptr<track_model::TrackModel>> track_model : tracks_)
    {
        model = track_model.second->GetTrainModel(train);

        if (nullptr != model)
        {
            break;
        }
    }

    return model;
}

types::Error Simulator::SetTrackCircuitData(const types::TrackCircuitData &data)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(data.track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else if (types::Error::ERROR_NONE != tracks_[data.track].get()->SetCommandedSpeed(data.block, data.speed))
    {
        error = types::Error::ERROR_INVALID_BLOCK;
    }
    else if (types::Error::ERROR_NONE != tracks_[data.track].get()->SetAuthority(data.block, data.authority))
    {
        error = types::Error::ERROR_INVALID_BLOCK;
    }

    return error;
}

types::Error Simulator::SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switched)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        error = tracks_[track]->SetSwitchState(block, switched);
    }

    return error;
}

types::Error Simulator::SetCrossingState(const types::TrackId track, const types::BlockId block, const bool closed)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        error = tracks_[track]->SetCrossingState(block, closed);
    }

    return error;
}

types::Error Simulator::SetTrafficLight(const types::TrackId track, const types::BlockId block, const types::TrafficLightColor color)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        switch (color)
        {
        case types::TrafficLightColor::TRAFFICLIGHTCOLOR_RED:
            error = tracks_[track]->SetRedTrafficLight(block, true);
            if (types::Error::ERROR_NONE == error)
            {
                error = tracks_[track]->SetGreenTrafficLight(block, false);
            }
            break;
        case types::TrafficLightColor::TRAFFICLIGHTCOLOR_GREEN:
            error = tracks_[track]->SetGreenTrafficLight(block, true);
            if (types::Error::ERROR_NONE == error)
            {
                error = tracks_[track]->SetRedTrafficLight(block, false);
            }
            break;
        default:
            error = types::Error::ERROR_INVALID_FORMAT;
            break;
        }
    }

    return error;
}

types::Error Simulator::GetBlockOccupancy(const types::TrackId track, const types::BlockId block, bool &occupied) const
{
    types::Error error = types::Error::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::Error::ERROR_INVALID_TRACK;
    }
    else
    {
        error = tracks_.find(track)->second->GetBlockOccupancy(block, occupied);
    }

    return error;
}

} // namespace simulator