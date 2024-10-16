#include "block_builder.h"
#include<bits/stdc++.h> 

BlockBuilder::BlockBuilder(void)
{
}

BlockBuilder::BlockBuilder(const std::vector<std::vector<std::string> > &records) 
{
    for (size_t i = 1; i < records.size(); ++i)
    {
        const auto& record = records[i];
        types::Block block  = ConvertRecordToBlock(record);
        blocks_.push_back(block);
    }
}

void AssignBlockInfrastructure(types::Block &block,const std::string &infrastructure) {
    std::string infra = infrastructure;
    std::transform(infra.begin(), infra.end(), infra.begin(), ::tolower);
    if (infra.find("station") != std::string::npos) 
    {
        block.has_station = true;
        block.station_name = infra;
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
}

types::Block BlockBuilder::ConvertRecordToBlock(const std::vector<std::string> &record)
{
    types::Block block;
    block.section = record[1][0];
    try {
        block.block = std::stoi(record[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block number: " << record[2] << std::endl;
        throw;
    }

    try {
        block.length = std::stod(record[3]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block length: " << record[3] << std::endl;
        throw;
    }

    try {
        block.grade = std::stod(record[4]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block grade: " << record[4] << std::endl;
        throw;
    }

    try {
        block.speed_limit = std::stod(record[5]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block speed limit: " << record[5] << std::endl;
        throw;
    }

    // Assigning Infrastructure to block
    AssignBlockInfrastructure(block, record[6]);

    try {
        std::string station_side = record[7];
        if (station_side.find("Left") != std::string::npos && station_side.find("Right") != std::string::npos) {
            block.station_side = types::StationSide::BOTH;
        }
        else if (station_side.find("Left") != std::string::npos)
        {
            block.station_side = types::StationSide::LEFT;
        }
        else if (station_side.find("Right") != std::string::npos)
        {
            block.station_side = types::StationSide::RIGHT;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block speed limit: " << record[5] << std::endl;
        throw;
    }
    
    try {
        block.elevation = std::stod(record[8]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for block elevation: " << record[7] << std::endl;
        throw;
    }

    try {
        block.cumulative_elevation = std::stod(record[9]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << "Block: " << record[2] << " Invalid input for cumulative elevation: " << record[8] << std::endl;
        throw;
    }
    return block;
}

types::Block BlockBuilder::GetBlock(const types::BlockId block_id) {
    types::Block block;
    for (auto& blk : blocks_)
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

