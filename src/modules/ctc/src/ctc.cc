/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "wayside_controller_gateway.h"
#include "ctc.h"

#include <sstream>
#include "unordered_map"
#include <iostream>


namespace ctc
{

ctc::Block ctc::Ctc::PopulateBlockFromRecord(std::vector<std::string> record)
{
    ctc::Block block;
    block.section      = record[1][0];
    block.block_number = std::stoi(record[2]);
    block.block_length = std::stoi(record[3]);
    block.block_grade  = std::stof(record[4]);
    block.speed_limit  = std::stoi(record[5]);
    AssignInfrastructure(record[6], block);
    block.elevation     = std::stof(record[7]);
    block.cum_elevation = std::stof(record[8]);
    return block;
}

void ctc::Ctc::PopulateDataFromCsv(std::vector<std::vector<std::string> > records)
{
    for (size_t i = 1; i < records.size(); ++i)
    {
        const auto& record = records[i];
        ctc::Block  block  = Ctc::PopulateBlockFromRecord(record);
        block_map.insert({record[2], block});
        if (block.infra.station)
        {
            PopulateTrainSchedule(record);
        }
    }
}

void ctc::Ctc::PopulateTrainSchedule(std::vector<std::string> record)
{
    std::vector<std::string> train_list        = SplitBySemicolon(record[9]);
    std::vector<std::string> arrival_time_list = SplitBySemicolon(record[10]);
    for (size_t i = 0; i < train_list.size(); i++)
    {
        auto it = train_schedule.find(train_list[i]);
        if ( it != train_schedule.end() )
        {
            it->second.insert({record[6], arrival_time_list[i]});
        }
        else
        {
            std::unordered_map<std::string, std::string> destination_and_arrival_time = {{record[6], arrival_time_list[i]}};
            train_schedule.insert({train_list[i], destination_and_arrival_time});
        }
    }
}

int AssignInfrastructure(const std::string& input, ctc::Block& block)
{
    if (input.find("RAILWAY") != std::string::npos)
    {
        block.infra.railway_crossing = true;
    }
    if (input.find("Switch") != std::string::npos)
    {
        block.infra.rail_switch = true;
    }
    if (input.find("transponder") != std::string::npos)
    {
        block.infra.transponder = true;
    }
    if (input.find("Station") != std::string::npos)
    {
        block.infra.station      = true;
        block.infra.station_name = input;
    }
    return 0;
}

size_t ctc::Ctc::PrintTrackMapSize(void) const
{
    return block_map.size();
}

void ctc::Ctc::AddTrainScheduleManual(std::string& train_name, std::string& destination, std::string& arrival_time)
{
    auto it = train_schedule.find(train_name);
    if ( it != train_schedule.end() )
    {
        it->second.insert({destination, arrival_time});
    }
    else
    {
        std::unordered_map<std::string, std::string> destination_and_arrival_time = {{destination, arrival_time}};
        train_schedule.insert({train_name, destination_and_arrival_time});
    }
}

void ctc::Ctc::PrintAllTrainSchedule(void) const
{
    for (const auto& train_entry : train_schedule)
    {
        // The outer map has train names as keys, and another unordered_map as values
        const std::string& train_name       = train_entry.first;
        const auto&        station_schedule = train_entry.second;

        std::cout << "Train: " << train_name << std::endl;

        // Now iterate through the station and arrival time map
        for (const auto& schedule_entry : station_schedule)
        {
            const std::string& station_name = schedule_entry.first;
            const std::string& arrival_time = schedule_entry.second;
            std::cout << "  Station: " << station_name << ", Arrival Time: " << arrival_time << std::endl;
        }

        std::cout << std::endl; // Just to space out train entries
    }
}

std::vector<std::string> SplitBySemicolon(const std::string& input)
{
    std::vector<std::string> result;
    std::stringstream        ss(input);
    std::string              token;

    // Split by ';' and push each part into the result vector
    while (std::getline(ss, token, ';'))
    {
        // Optionally, trim whitespace around the token
        token.erase(0, token.find_first_not_of(" \t\n\r")); // Trim leading whitespace
        token.erase(token.find_last_not_of(" \t\n\r") + 1); // Trim trailing whitespace

        result.push_back(token);
    }

    return result;
}

void ctc::Ctc::PrintBlockInfo(const ctc::Block& block) const
{
    std::cout << "Block Information: \n";
    std::cout << "-------------------\n";
    std::cout << "Block ID: " << block.block_id << "\n";
    std::cout << "Section: " << block.section << "\n";
    std::cout << "Block Number: " << block.block_number << "\n";
    std::cout << "Block Length: " << block.block_length << " meters\n";
    std::cout << "Block Grade: " << block.block_grade << "%\n";
    std::cout << "Speed Limit: " << block.speed_limit << " km/h\n";
    std::cout << "Elevation: " << block.elevation << " meters\n";
    std::cout << "Cumulative Elevation: " << block.cum_elevation << " meters\n";

    // Print Infrastructure details
    std::cout << "\nInfrastructure Details:\n";
    std::cout << "Station: " << (block.infra.station ? "Yes" : "No") << "\n";
    if (block.infra.station)
    {
        std::cout << "Station Name: " << block.infra.station_name << "\n";
    }
    std::cout << "Railway Crossing: " << (block.infra.railway_crossing ? "Yes" : "No") << "\n";
    std::cout << "Rail Switch: " << (block.infra.rail_switch ? "Yes" : "No") << "\n";
    std::cout << "Underground: " << (block.infra.underground ? "Yes" : "No") << "\n";
    std::cout << "Transponder: " << (block.infra.transponder ? "Yes" : "No") << "\n";
    std::cout << "-------------------\n";
}

void ctc::Ctc::PrintAllBlockMap(void) const
{
    std::cout << "Block Map Information: \n";
    std::cout << "-----------------------\n";

    for (const auto& pair : block_map)
    {
        const std::string& key   = pair.first;  // The key in the map (e.g., block name or ID)
        const Block&       block = pair.second; // The Block object

        std::cout << "Key: " << key << "\n";
        PrintBlockInfo(block);                // Print block information
        std::cout << "\n";                    // Add space between blocks
    }

    std::cout << "-----------------------\n";
}

void ctc::Ctc::SetBlockOccupancy(std::string block_number) {
    auto it = block_map.find(block_number);
    // Check if the block is found
    if (it != block_map.end()) {
        // Set occupancy to true
        it->second.occupancy = true;
        std::cout << "Occupancy set to true for block number: " << block_number << std::endl;
    } else {
        // Block with given block_number not found
        std::cerr << "Error: Block with number " << block_number << " not found." << std::endl;
    }
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC