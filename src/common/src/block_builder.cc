#include "block_builder.h"

#include <bits/stdc++.h>

static std::vector<std::string> SplitBySemicolon(const std::string& input);

BlockBuilder::BlockBuilder(void)
{
}

BlockBuilder::BlockBuilder(const std::vector<std::vector<std::string>> &records)
{
    for (size_t i = 1; i < records.size(); ++i)
    {
        const std::vector<std::string> &record = records[i];
        types::Block                    block  = ConvertRecordToBlock(record);
        blocks_.push_back(block);
    }
}

void BlockBuilder::AssignBlockInfrastructure(types::Block &block, const std::string &input_string)
{
    std::string input_lowercase = input_string;
    std::transform(input_lowercase.begin(), input_lowercase.end(), input_lowercase.begin(), ::tolower);

    std::vector<std::string> infrastructure_list = SplitBySemicolon(input_lowercase);
    for (std::size_t i = 0; i < infrastructure_list.size(); ++i)
    {
        const std::string &infrastructure = infrastructure_list[i];
        // Check for "station" keyword
        if ((infrastructure.find("station") != std::string::npos) && (i + 1 < infrastructure_list.size()))
        {
            block.has_station = true;
            std::string station_name = infrastructure_list[i + 1];
            station_name[0]    = std::toupper(station_name[0]);
            block.station_name = station_name;
        }
        if (infrastructure.find("railway") != std::string::npos)
        {
            block.has_crossing = true;
        }
        if (infrastructure.find("light") != std::string::npos)
        {
            block.has_light = true;
        }
        if (infrastructure.find("switch") != std::string::npos)
        {
            block.has_switch = true;
        }
        if (infrastructure.find("underground") != std::string::npos)
        {
            block.underground = true;
        }
    }
}

types::Block BlockBuilder::ConvertRecordToBlock(const std::vector<std::string> &record)
{
    types::Block block;
    block.section     = record[1][0];
    block.block       = std::stoi(record[BLOCK_BUILDER_CSV_FIELD_BLOCK_NUMBER]);
    block.length      = std::stod(record[BLOCK_BUILDER_CSV_FIELD_BLOCK_LENGTH]);
    block.grade       = std::stod(record[BLOCK_BUILDER_CSV_FIELD_BLOCK_GRADE]);
    block.speed_limit = std::stod(record[BLOCK_BUILDER_CSV_FIELD_SPEED_LIMIT]);
    AssignBlockInfrastructure(block, record[BLOCK_BUILDER_CSV_FIELD_INFRASTRUCTURE]);
    std::string station_side = record[BLOCK_BUILDER_CSV_FIELD_STATION_SIDE];
    if ((station_side.find("Left") != std::string::npos) && (station_side.find("Right") != std::string::npos))
    {
        block.station_side = types::StationSide::STATIONSIDE_BOTH;
    }
    else if (station_side.find("Left") != std::string::npos)
    {
        block.station_side = types::StationSide::STATIONSIDE_LEFT;
    }
    else if (station_side.find("Right") != std::string::npos)
    {
        block.station_side = types::StationSide::STATIONSIDE_RIGHT;
    }
    block.elevation            = std::stod(record[BLOCK_BUILDER_CSV_FIELD_ELEVATION]);
    block.cumulative_elevation = std::stod(record[BLOCK_BUILDER_CSV_FIELD_CUMULATIVE_ELEVATION]);

    return block;
}

types::Error BlockBuilder::GetBlock(const types::BlockId block_id, types::Block &block)
{
    types::Error                        error = types::ERROR_NONE;
    std::vector<types::Block>::iterator it    = std::find_if(blocks_.begin(), blocks_.end(), [&block_id](const types::Block& block) {
        return block.block == block_id;
    });

    // Check if block was found
    if (it != blocks_.end())
    {
        block = *it;  // Return the found block
    }
    else
    {
        error = types::ERROR_INVALID_BLOCK;
    }
    return error;
}

std::vector<types::Block> BlockBuilder::GetBlocks(void) const
{
    return blocks_;
}

std::size_t BlockBuilder::GetSize(void) const
{
    return blocks_.size();
}

void BlockBuilder::Reset(void)
{
    blocks_.clear();
}

static std::vector<std::string> SplitBySemicolon(const std::string& input)
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