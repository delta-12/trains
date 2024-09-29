#include "simulator.h"

#include <algorithm>

namespace simulator
{

void Simulator::AddTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    if (tracks_.end() == std::find(tracks_.begin(), tracks_.end(), track))
    {
        tracks_.push_back(track);
    }
}

void Simulator::DeleteTrackModel(const types::TrackId track)
{
    tracks_.erase(std::remove_if(tracks_.begin(), tracks_.end(), [track](std::shared_ptr<track_model::TrackModel> model){
            return model->GetTrackId() == track;
        }), tracks_.end());
}

void Simulator::GetTrackModels(std::vector<std::shared_ptr<track_model::TrackModel> > &tracks) const
{
    tracks = tracks_;
}

types::Error Simulator::AddTrainModel(const types::TrackId track, std::shared_ptr<train_model::TrainModel> train)
{
    types::Error                                                     error = types::ERROR_NONE;
    std::vector<std::shared_ptr<track_model::TrackModel> >::iterator i     = std::find_if(tracks_.begin(), tracks_.end(), [track](std::shared_ptr<track_model::TrackModel> model){
            return model->GetTrackId() == track;
        });

    if (tracks_.end() != i)
    {
        i->get()->AddTrainModel(train);
    }
    else
    {
        error = types::ERROR_INVALID_TRACK;
    }

    return error;
}

void Simulator::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const
{
    std::vector<std::shared_ptr<train_model::TrainModel> > track_trains;

    for (std::vector<std::shared_ptr<track_model::TrackModel> >::const_iterator i = tracks_.begin(); i != tracks_.end(); i++)
    {
        i->get()->GetTrainModels(track_trains);

        trains.insert(std::end(trains), std::begin(track_trains), std::end(track_trains));
    }
}

types::Error Simulator::Update(wayside_controller::Gateway &wayside_controller_gateway, train_controller::Gateway &train_controller_gateway)
{
    types::Error error = wayside_controller_gateway.UpdateTrackModels(tracks_);

    if (types::ERROR_NONE == error)
    {
        std::vector<std::shared_ptr<train_model::TrainModel> > trains;
        GetTrainModels(trains);
        error = train_controller_gateway.UpdateTrainModels(trains);
    }

    if (types::ERROR_NONE == error)
    {
        std::for_each(tracks_.begin(), tracks_.end(), [](std::shared_ptr<track_model::TrackModel> track){
                track->Update();
            });
    }

    return error;
}

} // namespace simulator