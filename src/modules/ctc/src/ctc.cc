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

ctc::Block ctc::Ctc::PopulateBlockFromRecord(std::vector<std::string> record) {
    ctc::Block block;
    block.section = record[1][0];
    block.block_number = std::stoi(record[2]);
    block.block_length = std::stoi(record[3]);
    block.block_grade = std::stof(record[4]);
    block.speed_limit = std::stoi(record[5]);
    AssignInfrastructure(record[6], block);
    block.elevation = std::stof(record[7]);
    block.cum_elevation = std::stof(record[8]);
    return block;
}

void ctc::Ctc::PopulateDataFromCsv(std::vector<std::vector<std::string>> records) {
    for (size_t i = 1; i < records.size(); ++i) {
        const auto& record = records[i];
        ctc::Block block = Ctc::PopulateBlockFromRecord(record);
        block_map.insert({record[2], block});
        if (block.infra.station) {
            PopulateTrainSchedule(record);
        }
    }
}

void ctc::Ctc::PopulateTrainSchedule(std::vector<std::string> record) {
    std::vector<std::string> train_list = SplitBySemicolon(record[9]);
    std::vector<std::string> arrival_time_list = SplitBySemicolon(record[10]);
    for (size_t i = 0; i < train_list.size(); i++) {
        auto it = train_schedule.find(train_list[i]);
        if( it != train_schedule.end() ) {
            it->second.insert({record[6], arrival_time_list[i]});
        }
        else {
            std::unordered_map<std::string, std::string> destination_and_arrival_time = {{record[6], arrival_time_list[i]}};
            train_schedule.insert({train_list[i], destination_and_arrival_time});
        }
    }
}

int AssignInfrastructure(const std::string& input, ctc::Block& block) {
    if (input.find("RAILWAY") != std::string::npos) {
        block.infra.railway_crossing = true;
    } 
    if (input.find("Switch") != std::string::npos) {
        block.infra.rail_switch = true;
    }
    if (input.find("transponder") != std::string::npos) {
        block.infra.transponder = true;
    }
    if (input.find("Station") != std::string::npos) {
        block.infra.station = true;
        block.infra.station_name = input;
    }
    return 0;
}

size_t ctc::Ctc::PrintTrackMapSize(void) const {
    return block_map.size();
}

void ctc::Ctc::PrintAllTrainSchedule(void) const {
    for (const auto& train_entry : train_schedule) {
        // The outer map has train names as keys, and another unordered_map as values
        const std::string& train_name = train_entry.first;
        const auto& station_schedule = train_entry.second;

        std::cout << "Train: " << train_name << std::endl;

        // Now iterate through the station and arrival time map
        for (const auto& schedule_entry : station_schedule) {
            const std::string& station_name = schedule_entry.first;
            const std::string& arrival_time = schedule_entry.second;

            std::cout << "  Station: " << station_name << ", Arrival Time: " << arrival_time << std::endl;
        }

        std::cout << std::endl; // Just to space out train entries
    }
}

std::vector<std::string> SplitBySemicolon(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;

    // Split by ';' and push each part into the result vector
    while (std::getline(ss, token, ';')) {
        // Optionally, trim whitespace around the token
        token.erase(0, token.find_first_not_of(" \t\n\r")); // Trim leading whitespace
        token.erase(token.find_last_not_of(" \t\n\r") + 1); // Trim trailing whitespace

        result.push_back(token);
    }

    return result;
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC