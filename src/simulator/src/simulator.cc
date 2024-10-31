#include "simulator.h"

#include <algorithm>

namespace simulator
{

types::Error Simulator::AddTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    types::Error   error    = types::ERROR_NONE;
    types::TrackId track_id = track.get()->GetTrackId();

    if (tracks_.contains(track_id))
    {
        error = types::ERROR_DUPLICATE_TRACK;
    }
    else
    {
        tracks_[track_id] = track;
    }

    return error;
}

types::Error Simulator::DeleteTrackModel(const types::TrackId track)
{
    types::Error error = types::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::ERROR_INVALID_TRACK;
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
    types::Error error = types::ERROR_NONE;

    if (!tracks_.contains(track))
    {
        error = types::ERROR_INVALID_TRACK;
    }
    else
    {
        tracks_[track].get()->AddTrainModel(train);
    }

    return error;
}

// void Simulator::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const
// {
//     std::vector<std::shared_ptr<train_model::TrainModel>> track_trains;

//     for (std::vector<std::shared_ptr<track_model::TrackModel>>::const_iterator i = tracks_.begin(); i != tracks_.end(); ++i)
//     {
//         i->get()->GetTrainModels(track_trains);

//         trains.insert(std::end(trains), std::begin(track_trains), std::end(track_trains));
//     }
// }

types::Error Simulator::SetTrackCircuitData(const types::TrackCircuitData &data)
{
    types::Error error = types::ERROR_NONE;

    if (!tracks_.contains(data.track))
    {
        error = types::ERROR_INVALID_TRACK;
    }
    else if (types::ERROR_NONE != tracks_[data.track].get()->SetCommandedSpeed(data.block, data.speed))
    {
        error = types::ERROR_INVALID_BLOCK;
    }
    else if (types::ERROR_NONE != tracks_[data.track].get()->SetAuthority(data.block, data.authority))
    {
        error = types::ERROR_INVALID_BLOCK;
    }

    return error;
}

} // namespace simulator