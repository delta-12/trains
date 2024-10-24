#include "track_model.h"

#include <iostream>
#include <string>
#include <random>

#include "train_model.h"
#include "types.h"

namespace track_model
{

//constructor for when a track is passsed in
types::Error SoftwareTrackModel::SetTrackLayout(const types::TrackId track, const std::vector<Block> &blocks)
{
    //setting trackID
    track_ = track;

    //fill vector with blocks
    blocks_ = blocks;
}

types::TrackId SoftwareTrackModel::GetTrackId(void)
{
    // Implementation logic to get the Track ID
    return track_;
}

types::Error SoftwareTrackModel::AddTrainModel(std::shared_ptr<train_model::TrainModel> train)
{
    // Implementation logic to add a train model

    //TODO: ADD FAILURE STATES

    //add model
    trains_.push_back(train);

    return types::ERROR_NONE;
}


void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const
{
    // Populating the trains vector with current train models
    trains = trains_;
}


void SoftwareTrackModel::Update(void)
{
    // Update the state of the track model

    //check temperature for heaters
    if (external_temperature_ <= 32)
    {
        //set all track heaters
        for (int i = 0; i < blocks_.size(); i++)
        {
            blocks_[i].heater_on = 1;
        }
    }

    //loop through all trains on the blocks_
    for (int i = 0; i < trains_.size(); i++)
    {
        //get the distance traveled of the train
        types::BlockId trainblock = trains_[i]->GetDistanceTraveled();

        //TODO: CALCULATE ALL BLOCKS THAT THIS TRAIN IS OCCUPYING
        types::BlockId front_of_train_block

        //TODO: clear old occupancies for this train specifically
        for (int k = 0; k < occupied_blocks_.size(); k++)
        {
            int pos = occupied_blocks_[k];
            blocks_[pos].occupied = 0;
        }

        //TODO: SET NEW OCCUPANCIES AND ADD THEM TO OCCPIED_BLOCKS_
        blocks_[trainblock].occupied = 1;

        //check the length of the block the train is occupying, and see if the train is longer
        if (blocks_[trainblock].length < tlength)
        {
            //looping until the full length of the train is accounted for
            auto sizeofblocks = blocks_[trainblock].length;
            int  j            = trainblock - 1;
            while (sizeofblocks < tlength)
            {
                //checking if the block behind this is connected to another block
                if (blocks_[j].switch_connection != 0)
                {
                    //adding size of block connected
                    types::BlockId blockcon = blocks_[j].switch_connection;

                    //adjust j
                    j = blockcon;
                }
                //adding the size of the block behind it
                sizeofblocks += blocks_[j - 1].length;

                //adding this block to the vector of occupancies
                blocks_[j].occupancy = 1;

                //adding to current train blocks
                trainblockvec.push_back(j);

                //increment
                j--;
            }
        }

        //if this train is at a station, update boarding

        if (blocks_[front_of_train_block].station == 1)
        {
            uint16_t traindeb = trains_[i]->GetPassengersDeboarding();
            SetPassengersDeboarding(trains_[i]->GetTrainId(), traindeb);
        }
    }

    //clear old occupancies
    for (int k = 0; k < trainblockvec.size(); k++)
    {
        int pos = trainblockvec[k];
        blocks_[pos].occupancy = 0;
    }

    //get the BlockId of the train
    types::BlockId trainblock = currblock;

    //set block occupancy
    blocks_[trainblock].occupied = 1;

    trainblockvec.push_back(trainblock);
}



types::Error SoftwareTrackModel::SetSwitchState(const types::BlockId block, const bool Switched)
{
    // Logic to set the Switch state for the specified block
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    blocks_[block].switched = Switched;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed)
{
    // Logic to set the crossing state for the specified block
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (blocks_[block].has_crossing != 1)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    blocks_[block].crossing_state = closed;

    return types::Error{ };
}

types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the red traffic light state for the specified block
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (blocks_[block].has_light != 1)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (on == 1)
    {
        blocks_[block].light_color = types::LIGHTCOLOR_RED;
    }
    else
    {
        blocks_[block].light_color = types::LIGHTCOLOR_NONE;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetYelloTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the yellow traffic light state for the specified block
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (blocks_[block].has_light != 1)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (on == 1)
    {
        blocks_[block].light_color = types::LIGHTCOLOR_NONE;
    }
    else
    {
        blocks_[block].light_color = types::LIGHTCOLOR_NONE;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the green traffic light state for the specified block
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (blocks_[block].has_light != 1)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (on == 1)
    {
        blocks_[block].light_color = types::LIGHTCOLOR_GREEN;
    }
    else
    {
        blocks_[block].light_color = types::LIGHTCOLOR_NONE;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)
{
    // Logic to set the commanded speed
    return types::Error{ };
}

types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Meters authority)
{
    // Logic to set authority
    return types::Error{  };
}

types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const
{
    // Logic to determine block occupancy and set the occupied variable

    //checking if block exists
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //check occupancy
    if (blocks_[block].occupied == 1)
    {
        //set occupancy variable
        occupied = 1;
    }
    else
    {
        occupied = 0;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken)
{
    // Logic to set the broken rail state for the specified block

    //checking if block exists
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //setting broken rail
    blocks_[block].broken_rail = broken;

    //occupancy
    blocks_[block].occupied = broken;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure)
{
    // Logic to set the track circuit failure state for the specified block

    //checking if block exists
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //setting TC fail
    blocks_[block].track_circuit_failure = track_circuit_failure;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure)
{
    // Logic to set the power failure state for the specified block

    //checking if block exists
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //setting TC fail
    blocks_[block].power_failure = power_failure;

    //occupancy
    blocks_[block].occupied = power_failure;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    //setting temperature
    external_temperature_ = temperature;

    if (temperature < 32)
    {
        for (int i = 0; i < blocks_.size(); i++)
        {
            blocks_[i].heater_on = 1;
        }
    }
    else
    {
        for (int i = 0; i < blocks_.size(); i++)
        {
            blocks_[i].heater_on = 0;
        }
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetTrainBlock(const types::BlockId block)
{
    //checking if block exists
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //probably gonna move the logic to assign block occupancy for trains here

    return types::ERROR_NONE;
}

//update commanded speed and authority every block

//is this getting callled only when deboarding is gonna happen?
types::Error SoftwareTrackModel::SetPassengersDeboarding(const types::TrainId train, const uint16_t passengers)
{
    //get the passengers deboarding
    const uint16_t deboarding = passengers;

    //subtract from total passengers
    uint16_t vacancy = 222 - passenger_counts_[train - 1] + deboarding;

    //generate random number within bounds for boarding
    std::random_device              rd;              // Seed
    std::mt19937                    gen(rd());       // Mersenne Twister engine
    std::uniform_int_distribution<> dis(0, vacancy); // Uniform distribution between 0 and board

    // Generate a random number
    int randomNumber = dis(gen);

    //set passengers boarding
    trains_[train - 1]->SetPassengersBoarding(randomNumber);

    //update passengers on board
    passenger_counts_[train - 1] = passenger_counts_[train - 1] - deboarding + randomNumber;

    return types::ERROR_NONE;
}

struct ::Block SoftwareTrackModel::GetBlock(const types::BlockId block)
{
    return blocks_[block];
}

} // namespace track_model