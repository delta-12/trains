#include "block_builder.h"
#include <bits/stdc++.h>

BlockBuilder::BlockBuilder(void)
{
}

BlockBuilder::BlockBuilder(const std::vector<std::vector<std::string> > &records)
{
    for (size_t i = 1; i < records.size(); ++i)
    {
        const std::vector<std::string> &record = records[i];
        types::Block                    block  = ConvertRecordToBlock(record);
        blocks_.push_back(block);
    }
}

static std::vector<std::string> SplitBySemicolon(const std::string& input);

void BlockBuilder::AssignBlockInfrastructure(types::Block &block, const std::string &input_string)
{
    std::string infra_string = input_string;
    std::transform(infra_string.begin(), infra_string.end(), infra_string.begin(), ::tolower);

    std::vector<std::string> infra_list = SplitBySemicolon(infra_string);
    for (std::size_t i = 0; i < infra_list.size(); ++i)
    {
        const std::string &infra = infra_list[i];
        // Check for "station" keyword
        if (infra.find("station") != std::string::npos)
        {
            block.has_station = true;

            // Ensure there's a next token for the station name
            if (i + 1 < infra_list.size())
            {
                std::string station_name = infra_list[i + 1];
                station_name[0]    = std::toupper(station_name[0]);
                block.station_name = station_name;  // The next token should be the station name
            }
        }
        if (infra.find("railway") != std::string::npos)
        {
            block.has_crossing = true;
        }
        if (infra.find("light") != std::string::npos)
        {
            block.has_light = true;
        }
        if (infra.find("switch") != std::string::npos)
        {
            block.has_switch = true;
        }
        if (infra.find("underground") != std::string::npos)
        {
            block.underground = true;
        }
    }
}

types::Block BlockBuilder::ConvertRecordToBlock(const std::vector<std::string> &record)
{
    types::Block block;
    block.section     = record[1][0];
    block.block       = std::stoi(record[CSV_FIELD_BLOCK_NUMBER]);
    block.length      = std::stod(record[CSV_FIELD_BLOCK_LENGTH]);
    block.grade       = std::stod(record[CSV_FIELD_BLOCK_GRADE]);
    block.speed_limit = std::stod(record[CSV_FIELD_SPEED_LIMIT]);
    AssignBlockInfrastructure(block, record[CSV_FIELD_INFRASTRUCTURE]);
    std::string station_side = record[CSV_FIELD_STATION_SIDE];
    if (station_side.find("Left") != std::string::npos && station_side.find("Right") != std::string::npos)
    {
        block.station_side = types::StationSide::STATIONSIDE_BOTH;
    }
    else
    if (station_side.find("Left") != std::string::npos)
    {
        block.station_side = types::StationSide::STATIONSIDE_LEFT;
    }
    else
    if (station_side.find("Right") != std::string::npos)
    {
        block.station_side = types::StationSide::STATIONSIDE_RIGHT;
    }
    block.elevation            = std::stod(record[CSV_FIELD_ELEVATION]);
    block.cumulative_elevation = std::stod(record[CSV_FIELD_CUMULATIVE_ELEVATION]);

    return block;
}

types::Block BlockBuilder::GetBlock(const types::BlockId block_id)
{
    types::Block block;
    for (const types::Block &blk : blocks_)
    {
        if (blk.block == block_id)
        {
            block = blk;
        }
    }
    return block;
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