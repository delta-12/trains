#include "track_model.h"

#include <iostream>
#include <string>
#include <random>

#include "train_model.h"
#include "types.h"
#include "block_builder.h"
#include "graph.h"

namespace track_model
{

//constructor for when a track is passsed in
types::Error SoftwareTrackModel::SetTrackLayout(const types::TrackId track, const std::vector<types::Block> &blocks)
{
    //setting trackID
    track_ = track;

    //fill vector with blocks
    blocks_ = blocks;

    //TODO: FILL GRAPH WITH BLOCKS HERE

    for (int i = 0; i < blocks_.size(); i++)
    {
        // every edge is really the length of the second block
        graph.AddEdge(i, i + 1, blocks_[i + 1].length);
    }

    return types::ERROR_NONE;
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


void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const
{
    // Populating the trains vector with current train models
    trains = trains_;
}


void SoftwareTrackModel::Update(void)
{

    for (int i = 0; i < trains_.size(); i++)
    {
        types::Meters  new_distance = trains_[i]->GetDistanceTraveled();
        types::Meters  d_iterator   = 0;
        types::BlockId currblock    = 0;

        // Find the current block based on the distance traveled
        for (currblock = occupied_train_blocks_[i][0]; currblock < blocks_.size(); currblock++)
        {
            if (blocks_[currblock - 1].switched == 1)
            {
                currblock = blocks_[currblock].switch_connection;
            }

            d_iterator += blocks_[currblock].length;

            if (d_iterator >= new_distance)
            {
                break; // Train is in the current block
            }
        }

        // If the train is beyond the last block, we set currblock to the last block
        if (currblock >= blocks_.size())
        {
            currblock = blocks_.size() - 1;
        }

        // Clear old occupancies for this train
        for (int m = 0; m < occupied_train_blocks_[i].size(); m++)
        {
            types::BlockId occupied_spot = occupied_train_blocks_[i][m];
            blocks_[occupied_spot].occupied = 0;
        }
        occupied_train_blocks_[i].clear();

        // Update the current block occupancy
        blocks_[currblock].occupied = 1;
        occupied_train_blocks_[i].push_back(currblock);

        // Check if the train length occupies more than just the current block
        types::Meters  sizeofblocks = blocks_[currblock].length;
        types::BlockId j            = currblock - 1;

        // Loop backwards to account for the full length of the train
        while (sizeofblocks < train_length_ && j >= 0)
        {
            if (blocks_[j].switched == 1)
            {
                // Handle switched block
                for (int k = 0; k < blocks_.size(); k++)
                {
                    if (blocks_[k].switch_connection == currblock)
                    {
                        j = k;
                    }
                }
                if (j < 0 || j >= blocks_.size())
                {
                    break; // Prevent out-of-bounds
                }
            }

            // Add the size of the block and mark it as occupied
            sizeofblocks       += blocks_[j].length;
            blocks_[j].occupied = 1;
            occupied_train_blocks_[i].push_back(j);
            j--;
        }

        // Check if the current block has a station and update deboarding
        if (blocks_[currblock].has_station == 1)
        {
            uint16_t traindeb = trains_[i]->GetPassengersDeboarding();
            SetPassengersDeboarding(trains_[i]->GetTrainId(), traindeb);
        }
    }
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

    //TODO: UPDATE THE GRAPH

    if (Switched == 1)
    {
        //remove edge (previous connection)
        graph.RemoveEdge(block, block + 1);

        //add new edge (new connection)
        types::BlockId newcon = blocks_[block].switch_connection;
        graph.AddEdge(block, newcon, blocks_[newcon].length);
    }
    else
    {
        //remove edge (previous connection)
        types::BlockId oldcon = blocks_[block].switch_connection;
        graph.RemoveEdge(block, oldcon);

        //add new edge (new connection)
        graph.AddEdge(block, block + 1, blocks_[block + 1].length);
    }

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

types::Error SoftwareTrackModel::SetYellowTrafficLight(const types::BlockId block, const bool on)
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

    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //loop through train models
    for (int i = 0; i < trains_.size(); i++)
    {
        //loop through blocks the train is on
        for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
        {
            if (occupied_train_blocks_[i][j] == block)
            {
                //pass commanded speed to train
                trains_[i]->SetCommandedSpeed(speed);

                return types::ERROR_NONE;
            }
        }
    }

    return types::ERROR_INVALID_BLOCK;
}

types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Blocks authority)
{
    // Logic to set the authority

    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }


    //loop through train models
    for (int i = 0; i < trains_.size(); i++)
    {
        //loop through blocks the train is on
        for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
        {
            if (occupied_train_blocks_[i][j] == block)
            {
                //pass authority to train
                trains_[i]->SetAuthority(authority);

                return types::ERROR_NONE;
            }
        }
    }

    return types::ERROR_INVALID_BLOCK;
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

    occupied = blocks_[block].occupied;

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

    //setting block occupancy
    blocks_[block].occupied = track_circuit_failure;

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

    //setting power fail
    blocks_[block].power_failure = power_failure;

    //occupancy
    blocks_[block].occupied = power_failure;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    //setting temperature
    external_temperature_ = temperature;

    if (temperature <= 32)
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
    std::uniform_int_distribution<> dis(0, vacancy); // Uniform distribution between 0 and vacancy

    // Generate a random number
    int randomNumber = dis(gen);

    //set passengers boarding
    trains_[train - 1]->SetPassengersBoarding(randomNumber);

    //update passengers on board
    passenger_counts_[train - 1] = passenger_counts_[train - 1] - deboarding + randomNumber;

    return types::ERROR_NONE;
}

types::Block SoftwareTrackModel::GetBlock(const types::BlockId block)
{
    return blocks_[block];
}

std::vector<std::vector<types::BlockId>> SoftwareTrackModel::GetOccupiedTrainBlocks(void)
{
    return occupied_train_blocks_;
}

} // namespace track_model