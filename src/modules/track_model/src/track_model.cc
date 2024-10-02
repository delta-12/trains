#include "track_model.h"

namespace track_model
{
    //cmake --build build -t target

    /*types::TrackId SoftwareTrackModel::GetTrackId(void) {
    // Implementation logic to get the Track ID
    // Return a dummy TrackId for demonstration
    return types::TrackId{ };
    }

    types::Error SoftwareTrackModel::AddTrainModel(std::shared_ptr<train_model::TrainModel> train) {
        // Implementation logic to add a train model
        // Return an appropriate Error value based on the operation
        return types::Error{  };
    }

    void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const {
        // Populate the trains vector with current train models
    }

    void SoftwareTrackModel::Update(void) {
        // Update the state of the track model
    }

    types::Error SoftwareTrackModel::SetSwitchState(const types::BlockId block, const bool switched) {
        // Logic to set the switch state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed) {
        // Logic to set the crossing state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the red traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetYelloTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the yellow traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the green traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed) {
        // Logic to set the commanded speed for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Meters authority) {
        // Logic to set authority for the specified block
        return types::Error{  };
    }*/

    types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const {
        // Logic to determine block occupancy and set the occupied variable

        //checking if block exists
        if (line.size()<block)
        {
            occupied=0;
            return types::ERROR_INVALID_BLOCK;
        }

        //check occupancy
        if (line[block].occupancy==1)
        {
            //set occupancy variable
            occupied=1;
        }
        else
        {
            occupied=0;
        }

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken) {
        // Logic to set the broken rail state for the specified block

        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting broken rail
        line[block].brail=broken;

        //occupancy
        line[block].occupancy=1;

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure) {
        // Logic to set the track circuit failure state for the specified block
        
        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting TC fail
        line[block].tcfail=track_circuit_failure;

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure) {
        // Logic to set the power failure state for the specified block
        
        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting TC fail
        line[block].pfail=power_failure;

        //occupancy
        line[block].occupancy=1;

        return types::ERROR_NONE;
    }

} // namespace track_model