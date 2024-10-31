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
types::Error SoftwareTrackModel::SetTrackLayout(const types::TrackId track, const std::vector<types::Block> &blocks, const std::vector<types::Block> &inorder)
{
    //setting trackID
    track_ = track;

    //fill vector with blocks
    track_path_ = blocks;

    //fill blocks_ vector
    blocks_ = inorder;

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

    //populate occupied train block 2d vector with a new vector for this train
    occupied_train_blocks_.push_back({0});

    current_train_block_.push_back(0);

    train_head_.push_back(0);

    //populate passenger count vector
    passenger_counts_.push_back(0);

    return types::ERROR_NONE;
}


void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const
{
    // Populating the trains vector with current train models
    trains = trains_;
}


void SoftwareTrackModel::Update(void)
{
    //loop through train models
    for (int i = 0; i < trains_.size(); i++)
    {
        //fetch distance traveled
        types::MetersPerSecond d_traveled = trains_[i]->GetDistanceTraveled();

        types::MetersPerSecond temp_distance = 0;

        types::BlockId current_block;

        temp_distance += train_head_[i];

        //unoccupy old blocks
        for (int k = 0; k < occupied_train_blocks_[i].size(); k++)
        {
            types::BlockId oldblock = occupied_train_blocks_[i][k];
            blocks_[oldblock].occupied = 0;
        }
        //clear occupancies
        occupied_train_blocks_[i].clear();

        //traverse graph from current block to account for this length
        for (int j = current_train_block_[i]; j < track_path_.size(); j++)
        {
            //green line end switch check
            if (j == 172 && blocks_[57].switched == 1)
            {
                //block 57 now has the yard after it, and the front of the train is now at the yard
                j = 0;
                RemoveTrainModel(i);
                break;
            }
            else if (j == 175)
            {
                //block 57 now has J after it, which connects to K (starting a new loop)
                j = 1;
            }

            //add distance
            current_block = track_path_[j].block;

            if (j != current_train_block_[i])
            {
                temp_distance += blocks_[current_block].length;
            }

            //check if we have accounted for the distance traveled yet
            if (temp_distance >= d_traveled)
            {
                //update new block occupancy
                occupied_train_blocks_[i].push_back(current_block);
                blocks_[current_block].occupied = 1;

                current_train_block_[i] = j;

                //calculate head
                train_head_[i] = temp_distance - d_traveled;

                //loop to account for length of train
                types::Meters current_length = blocks_[current_block].length - train_head_[i];

                while (current_length < 32)
                {
                    //did we hit the yard?
                    if (track_path_[j].block == 0)
                    {
                        break;
                    }

                    //decrement to next block
                    j--;
                    current_length += track_path_[j].length;

                    //update occupany
                    types::BlockId next_block = track_path_[j].block;
                    occupied_train_blocks_[i].push_back(next_block);
                    blocks_[next_block].occupied = 1;

                }

                break;
            }

        }

        // Check if the current block has a station and update deboarding
        for (int m = 0; m < occupied_train_blocks_[i].size(); m++)
        {
            types::BlockId station_check = occupied_train_blocks_[i][m];

            if (blocks_[station_check].has_station == 1)
            {
                uint16_t traindeb = trains_[i]->GetPassengersDeboarding();
                SetPassengersDeboarding(i, traindeb);
            }
        }
    }
    //TODO: OCCUPY THE BLOCKS BEHIND THE TRAIN BLOCK IF THE LENGTH OF TRAIN > LENGTH OF CURRENT BLOCK

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

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on)
{
    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)
{
    if (blocks_.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //loop through trains
    for (int i = 0; i < trains_.size(); i++)
    {
        //does speed get sent to the block the (front of the) train is on?
        for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
        {
            if (occupied_train_blocks_[i][j] == block)
            {
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

    //loop through trains
    for (int i = 0; i < trains_.size(); i++)
    {
        //does authority get sent to the block the (front of the) train is on?
        for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
        {
            //std::cout << std::endl << occupied_train_blocks_[i][j] << std::endl;
            if (occupied_train_blocks_[i][j] == block)
            {
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

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure)
{
    // Logic to set the track circuit failure state for the specified block

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure)
{
    // Logic to set the power failure state for the specified block

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    return types::ERROR_NONE;
}

//is this getting callled only when deboarding is gonna happen?
types::Error SoftwareTrackModel::SetPassengersDeboarding(const types::TrainId train, const uint16_t passengers)
{
    //get the passengers deboarding
    const uint16_t deboarding = passengers;

    //subtract from total passengers
    uint16_t vacancy = 222 - passenger_counts_[train] + deboarding;

    //generate random number within bounds for boarding
    std::random_device              rd;              // Seed
    std::mt19937                    gen(rd());       // Mersenne Twister engine
    std::uniform_int_distribution<> dis(0, vacancy); // Uniform distribution between 0 and vacancy

    // Generate a random number
    int randomNumber = dis(gen);

    //set passengers boarding
    trains_[train]->SetPassengersBoarding(randomNumber);

    //update passengers on board
    passenger_counts_[train] = passenger_counts_[train] - deboarding + randomNumber;

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

types::Error SoftwareTrackModel::RemoveTrainModel(int train_element)
{
    if (trains_.size() <= train_element)
    {
        return types::ERROR_INVALID_TRAIN;
    }

    trains_.erase(trains_.begin() + train_element);

    return types::ERROR_NONE;
}

} // namespace track_model