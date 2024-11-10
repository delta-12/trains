#include "track_model.h"

#include <iostream>
#include <string>
#include <random>

#include "train_model.h"
#include "types.h"
#include "block_builder.h"
#include "random_number_generator.h"
#include "logger.h"

namespace track_model
{

const types::Meters  kTrainLength     = 32;
const uint16_t       kMaximumCapacity = 222;
const types::BlockId kPathEnd         = 172;
const types::BlockId kYardSwitch      = 57;
const types::BlockId kLoopAround      = 175;

// constructor for when a track is passsed in
types::Error SoftwareTrackModel::SetTrackLayout(const types::TrackId track, const std::vector<types::Block> &blocks, const std::vector<types::Block> &inorder)
{
    track_ = track;

    track_path_ = blocks;

    blocks_ = inorder;

    return types::Error::ERROR_NONE;
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

    return types::Error::ERROR_NONE;
}

std::shared_ptr<train_model::TrainModel> SoftwareTrackModel::GetTrainModel(const types::TrainId train) const
{
    std::shared_ptr<train_model::TrainModel> selected_train;

    for (size_t i = 0; i < trains_.size(); i++)
    {
        if (train == trains_[i]->GetTrainId())
        {
            selected_train = trains_[i];
        }
    }

    return selected_train;
}

void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains)
{
    trains = trains_;
}

void SoftwareTrackModel::Update(void)
{
    for (size_t i = 0; i < trains_.size(); i++)
    {
        types::MetersPerSecond d_traveled = trains_[i]->GetDistanceTraveled();

        types::MetersPerSecond temp_distance = 0;

        types::BlockId current_block;

        temp_distance += train_head_[i];

        // unoccupy old blocks
        for (size_t k = 0; k < occupied_train_blocks_[i].size(); k++)
        {
            types::BlockId oldblock = occupied_train_blocks_[i][k];
            blocks_[oldblock].occupied = 0;
        }

        occupied_train_blocks_[i].clear();

        // traverse graph from current block to account for this length
        for (size_t j = current_train_block_[i]; j < track_path_.size(); j++)
        {
            // green line end switch check
            if (j == kPathEnd && blocks_[kYardSwitch].switched == 1)
            {
                // block 57 now has the yard after it, and the front of the train is now at the yard
                RemoveTrainModel(i);
                break;
            }
            else if (j == kLoopAround)
            {
                // block 57 now has J after it, which connects to K (starting a new loop)
                j = 1;
            }

            // add distance
            current_block = track_path_[j].block;

            if (j != current_train_block_[i])
            {
                temp_distance += blocks_[current_block].length;
            }

            // check if we have accounted for the distance traveled yet
            if (temp_distance >= d_traveled)
            {
                // update new block occupancy
                occupied_train_blocks_[i].push_back(current_block);
                blocks_[current_block].occupied = 1;

                current_train_block_[i] = j;

                trains_[i]->SetTrackPolarity(track_path_[j].polarity);

                train_head_[i] = temp_distance - d_traveled;

                // loop to account for length of train
                types::Meters current_length = blocks_[current_block].length - train_head_[i];

                while (current_length < kTrainLength)
                {
                    // did we hit the yard?
                    if (track_path_[j].block == 0)
                    {
                        break;
                    }

                    // decrement to next block
                    j--;
                    current_length += track_path_[j].length;

                    // update occupany
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
    bool isValid = blocks_.size() > block && block > 0 && blocks_[block].has_switch == 1;
    if (isValid)
    {
        blocks_[block].switched = switched;
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_BLOCK;
}

types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(closed);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(on);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetYellowTrafficLight(const types::BlockId block, const bool on)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(on);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(on);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)
{
    bool isValid = blocks_.size() > block && block > 0;

    if (isValid)
    {
        for (size_t i = 0; i < trains_.size(); i++)
        {
            for (size_t j = 0; j < occupied_train_blocks_[i].size(); j++)
            {
                if (occupied_train_blocks_[i][j] == block)
                {
                    trains_[i]->SetCommandedSpeed(speed);
                }
            }
        }
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_BLOCK;
}

types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Blocks authority)
{
    bool isValid = blocks_.size() > block && block > 0;

    if (isValid)
    {
        for (size_t i = 0; i < trains_.size(); i++)
        {
            for (size_t j = 0; j < occupied_train_blocks_[i].size(); j++)
            {
                if (occupied_train_blocks_[i][j] == block)
                {
                    trains_[i]->SetAuthority(authority);
                }
            }
        }
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_BLOCK;
}

types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const
{
    // checking if block exists
    bool isValid = blocks_.size() > block && block > 0;

    if (isValid)
    {
        occupied = blocks_[block].occupied;
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_BLOCK;
}

types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(broken);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(track_circuit_failure);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure)
{
    LOGGER_UNUSED(block);
    LOGGER_UNUSED(power_failure);
    return types::Error::ERROR_NONE;
    //NNF233
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    LOGGER_UNUSED(temperature);
    return types::Error::ERROR_NONE;
    //NNF233
}

// is this getting callled only when deboarding is gonna happen?
types::Error SoftwareTrackModel::SetPassengersDeboarding(const types::TrainId train, const uint16_t passengers)
{
    const uint16_t deboarding = passengers;

    // subtract from total passengers
    uint16_t vacancy = kMaximumCapacity - passenger_counts_[train] + deboarding;

    RandomNumberGenerator rng;

    uint16_t randomNumber = rng.generate(vacancy);

    trains_[train]->SetPassengersBoarding(randomNumber);

    passenger_counts_[train] = passenger_counts_[train] - deboarding + randomNumber;

    return types::Error::ERROR_NONE;
}

std::vector<std::vector<types::BlockId>> SoftwareTrackModel::GetOccupiedTrainBlocks(void)
{
    return occupied_train_blocks_;
}

types::Error SoftwareTrackModel::RemoveTrainModel(size_t train_element)
{
    bool isValid = trains_.size() > train_element;

    if (isValid)
    {
        trains_.erase(trains_.begin() + train_element);

        occupied_train_blocks_.erase(occupied_train_blocks_.begin() + train_element);

        current_train_block_.erase(current_train_block_.begin() + train_element);

        train_head_.erase(train_head_.begin() + train_element);

        passenger_counts_.erase(passenger_counts_.begin() + train_element);
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_TRAIN;
}

types::Error SoftwareTrackModel::GetBlock(types::BlockId block_number, types::Block &block)
{
    bool isValid = block_number > 0 && blocks_.size() > block_number;

    if (isValid)
    {
        block = blocks_[block_number];
    }

    return isValid ? types::Error::ERROR_NONE : types::Error::ERROR_INVALID_BLOCK;
}

} // namespace track_model