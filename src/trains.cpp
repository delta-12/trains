#include <filesystem>
#include <unordered_map>

#include "ctc.h"
#include "simulator.h"
#include "track_model.h"
#include "train_controller_gateway.h"
#include "train_model.h"
#include "types.h"
#include "wayside_controller_gateway.h"

class SoftwarePort : public types::Port
{
public:
    void Send(const std::vector<uint8_t> &buffer)
    {
        buffers_.push_back(buffer);
    }

    void Receive(std::vector<uint8_t> &buffer)
    {
        buffer = buffers_[0];
        buffers_.erase(buffers_.begin());
    }

private:
    std::vector<std::vector<uint8_t>> buffers_; // Fake queue
};

std::shared_ptr<track_model::TrackModel> BuildTrackModel()
{
    return std::make_shared<track_model::TrackModel>();
}

std::shared_ptr<train_model::TrainModel> BuildTrainModel(const types::TrainId train)
{
    return std::make_shared<train_model::TrainModel>(train);
}

int main(void)
{
    ctc::Ctc ctc;
    simulator::Simulator world;
    wayside_controller::Gateway wayside_controller_gateway;
    train_controller::Gateway train_controller_gateway;

    std::shared_ptr<SoftwarePort> wayside_controller_port = std::make_shared<SoftwarePort>();
    std::shared_ptr<SoftwarePort> train_controller_port = std::make_shared<SoftwarePort>();

    wayside_controller_gateway.AttachPort(wayside_controller_port);
    train_controller_gateway.AttachPort(train_controller_port);

    // TODO instantiate wayside controller, inject wayside_controller_port
    // TODO instantiate train controller, inject train_controller_port

    // Add track model
    std::filesystem::path path("/foo/bar/baz.csv");
    world.AddTrackModel(BuildTrackModel());

    // Dispatch train
    // 1. CTC needs list of available train IDs from train controller gateway
    std::vector<types::TrainId> trains;
    train_controller_gateway.GetAvailableTrains(trains);
    // 2. CTC creates new train model w/ selected ID, then adds to track model
    types::TrackId track = 0;
    types::TrainId train = 1;
    world.AddTrainModel(track, BuildTrainModel(train));
    // 3. CTC sends dispatch signal to train controller
    train_controller_gateway.DispatchTrain(train);

    while (true)
    {
        // Udpdate wayside controller gateway and CTC
        ctc.UpdateWaysideControllers(wayside_controller_gateway);

        // Update wayside controllers
        wayside_controller_gateway.UpdateControllers();

        // Update train controllers
        train_controller_gateway.UpdateControllers();

        // Update simulation of the physical world
        world.Update(wayside_controller_gateway, train_controller_gateway);
    }

    return 0;
}