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

    //populate vector for blocks visited
    blocks_visited_.push_back({0});

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

        //find all blocks connected to the block the train is currently on
        auto connections = graph.BreadthFirstSearch(occupied_train_blocks_[i][0]);

        std::vector<types::BlockId> connections_vector(connections.begin(), connections.end());

        std::reverse(connections_vector.begin(), connections_vector.end());

        //traverse graph from current block to account for this length
        for (const auto& element : connections_vector)
        {
            if (element > occupied_train_blocks_[i][0])
            {
                //add distance
                current_block = element;

                temp_distance += blocks_[current_block].length;

                //check if we have accounted for the distance traveled yet
                if (temp_distance >= d_traveled)
                {
                    //unoccupy old block
                    types::BlockId oldblock = occupied_train_blocks_[i][0];
                    blocks_[oldblock].occupied = 0;

                    //update new block occupancy
                    occupied_train_blocks_[i][0]    = current_block;
                    blocks_[current_block].occupied = 1;
                    blocks_visited_[i].insert(blocks_visited_[i].begin(), current_block);

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

    //update graph
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