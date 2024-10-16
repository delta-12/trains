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
    block.block = std::stoi(record[2]);
    block.length = std::stod(record[3]);
    block.grade = std::stod(record[4]);
    block.speed_limit = std::stod(record[5]);
    // Assigning Infrastructure to block
    AssignBlockInfrastructure(block, record[6]);
    block.elevation = std::stod(record[7]);
    block.cumulative_elevation = std::stod(record[8]);
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

