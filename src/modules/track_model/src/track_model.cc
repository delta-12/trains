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

    //populate occupied train block 2d vector with a new vector for this train
    occupied_train_blocks_.push_back({0});

    //populate passenger count vector
    passenger_counts_.push_back(0);

    blocks_visited_.push_back({0});

    train_head_block_length_.push_back(0);
    //train_tail_length_.push_back(0);

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

        types::BlockId current_block = occupied_train_blocks_[i][0];

        //find all blocks connected to the block the train is currently on
        auto connections = graph.BreadthFirstSearch(occupied_train_blocks_[i][0]);

        std::vector<types::BlockId> connections_vector(connections.begin(), connections.end());

        std::reverse(connections_vector.begin(), connections_vector.end());

        //first, account for the rest of the length of the block that the head of the train is on
        temp_distance += train_head_block_length_[i];

        //traverse graph from current block to account for this length
        for (const auto& element : connections_vector)
        {
            if (element > occupied_train_blocks_[i][0])
            {
                //add distance
                current_block = element;

                temp_distance += blocks_[current_block].length;

                blocks_visited_[i].insert(blocks_visited_[i].begin(), current_block);

                //check if we have accounted for the distance traveled yet
                if (temp_distance >= d_traveled)
                {
                    //unoccupy old blocks
                    std::vector<types::BlockId> oldblocks = occupied_train_blocks_[i];
                    for (int j = 0; j < oldblocks.size(); j++)
                    {
                        blocks_[oldblocks[j]].occupied = 0;
                    }
                    occupied_train_blocks_[i].clear();

                    //update new block occupancy
                    occupied_train_blocks_[i]       = {current_block};
                    blocks_[current_block].occupied = 1;

                    //account for full length of train
                    train_head_block_length_[i] = temp_distance - d_traveled;

                    //initialize train length to be the portion of the current block that the train takes up
                    types::MetersPerSecond train_length = blocks_[current_block].length - train_head_block_length_[i];

                    size_t j = 1;

                    std::cout << std::endl << train_length << std::endl;

                    for (int k = 0; k < blocks_visited_[i].size(); k++)
                    {
                        std::cout << std::endl << blocks_visited_[i][k] << std::endl;
                    }

                    while (train_length < 32)
                    {
                        //go to the block behind it (if the train has actually visited that block before)
                        if (blocks_visited_[i].size() >= j)
                        {
                            train_length       += blocks_visited_[i][j];
                            blocks_[j].occupied = 1;
                            occupied_train_blocks_[i].push_back(j);

                            j++;

                        }
                        else //back of train is at yard
                        {
                            break;
                        }
                    }

                    break;
                }
            }

        }

        // Check if the current block has a station and update deboarding
        types::BlockId station_check = occupied_train_blocks_[i][0];

        if (blocks_[station_check].has_station == 1)
        {
            uint16_t traindeb = trains_[i]->GetPassengersDeboarding();
            SetPassengersDeboarding(i, traindeb);
        }

        //TODO: OCCUPY THE BLOCKS BEHIND THE TRAIN BLOCK IF THE LENGTH OF TRAIN > LENGTH OF CURRENT BLOCK
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
        if (occupied_train_blocks_[i][0] == block)
        {
            trains_[i]->SetCommandedSpeed(speed);

            return types::ERROR_NONE;
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
        if (occupied_train_blocks_[i][0] == block)
        {
            trains_[i]->SetAuthority(authority);

            return types::ERROR_NONE;
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

} // namespace track_model