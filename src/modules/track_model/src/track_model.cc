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
    track_ = track;

    track_path_ = blocks;

    blocks_ = inorder;

    return types::ERROR_NONE;
}

types::TrackId SoftwareTrackModel::GetTrackId(void)
{
    return track_;
}

types::Error SoftwareTrackModel::AddTrainModel(std::shared_ptr<train_model::TrainModel> train)
{
    trains_.push_back(train);

    occupied_train_blocks_.push_back({0});

    current_train_block_.push_back(0);

    train_head_.push_back(0);

    passenger_counts_.push_back(0);

    return types::ERROR_NONE;
}


void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const
{
    trains = trains_;
}


void SoftwareTrackModel::Update(void)
{
    for (int i = 0; i < trains_.size(); i++)
    {
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

        occupied_train_blocks_[i].clear();

        //traverse graph from current block to account for this length
        for (int j = current_train_block_[i]; j < track_path_.size(); j++)
        {
            //green line end switch check
            if (j == 172 && blocks_[57].switched == 1)
            {
                //block 57 now has the yard after it, and the front of the train is now at the yard
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

                trains_[i]->SetTrackPolarity(track_path_[j].polarity);

                train_head_[i] = temp_distance - d_traveled;

                //loop to account for length of train
                types::Meters current_length = blocks_[current_block].length - train_head_[i];

                while (current_length < length_of_train)
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
        types::BlockId possible_station_block = current_train_block_[i];
        if (blocks_[possible_station_block].has_station == 1)
        {
            uint16_t traindeb = trains_[i]->GetPassengersDeboarding();
            SetPassengersDeboarding(i, traindeb);
        }
    }

}



types::Error SoftwareTrackModel::SetSwitchState(const types::BlockId block, const bool switched)
{
    if (blocks_.size() > block && block > 0 && blocks_[block].has_switch == 1)
    {
        blocks_[block].switched = switched;

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_BLOCK;
    }

}

types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed)
{
    return types::Error{ };
}

types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on)
{
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
    if (blocks_.size() > block && block > 0)
    {
        for (int i = 0; i < trains_.size(); i++)
        {
            for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
            {
                if (occupied_train_blocks_[i][j] == block)
                {
                    trains_[i]->SetCommandedSpeed(speed);
                }
            }
        }

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_BLOCK;
    }
}

types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Blocks authority)
{
    if (blocks_.size() > block && block > 0)
    {
        for (int i = 0; i < trains_.size(); i++)
        {
            for (int j = 0; j < occupied_train_blocks_[i].size(); j++)
            {
                if (occupied_train_blocks_[i][j] == block)
                {
                    trains_[i]->SetAuthority(authority);
                }
            }

        }

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_BLOCK;
    }
}

types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const
{
    //checking if block exists
    if (blocks_.size() > block && block > 0)
    {
        occupied = blocks_[block].occupied;

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_BLOCK;
    }
}

types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken)
{
    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure)
{
    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure)
{
    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    return types::ERROR_NONE;
}

//is this getting callled only when deboarding is gonna happen?
types::Error SoftwareTrackModel::SetPassengersDeboarding(const types::TrainId train, const uint16_t passengers)
{
    if (trains_.size() > train)
    {
        const uint16_t deboarding = passengers;

        //subtract from total passengers
        uint16_t vacancy = train_capacity - passenger_counts_[train] + deboarding;

        //generate random number within bounds for boarding
        std::random_device              rd;              // Seed
        std::mt19937                    gen(rd());       // Mersenne Twister engine
        std::uniform_int_distribution<> dis(0, vacancy); // Uniform distribution between 0 and vacancy

        int randomNumber = dis(gen);

        trains_[train]->SetPassengersBoarding(randomNumber);

        passenger_counts_[train] = passenger_counts_[train] - deboarding + randomNumber;

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_TRAIN;
    }
}

std::vector<std::vector<types::BlockId>> SoftwareTrackModel::GetOccupiedTrainBlocks(void)
{
    return occupied_train_blocks_;
}

types::Error SoftwareTrackModel::RemoveTrainModel(int train_element)
{
    if (trains_.size() > train_element)
    {
        trains_.erase(trains_.begin() + train_element);

        occupied_train_blocks_.erase(occupied_train_blocks_.begin() + train_element);

        current_train_block_.erase(current_train_block_.begin() + train_element);

        train_head_.erase(train_head_.begin() + train_element);

        passenger_counts_.erase(passenger_counts_.begin() + train_element);

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_TRAIN;
    }
}

types::Error SoftwareTrackModel::GetBlock(types::BlockId block_number, types::Block &block)
{
    if (block_number > 0 && blocks_.size() > block_number)
    {
        block = blocks_[block_number];

        return types::ERROR_NONE;
    }
    else
    {
        return types::ERROR_INVALID_BLOCK;
    }
}

} // namespace track_model