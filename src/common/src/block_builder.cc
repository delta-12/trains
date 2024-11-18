#include "block_builder.h"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string>

#include "convert.h"
#include "types.h"

static const size_t kLayoutFieldBlockNumber          = 2;
static const size_t kLayoutFieldBlockLength          = 3;
static const size_t kLayoutFieldBlockGrade           = 4;
static const size_t kLayoutFieldSpeedLimit           = 5;
static const size_t kLayoutFieldInfrastructure       = 6;
static const size_t kLayoutFieldStationSide          = 7;
static const size_t kLayoutFieldElevation            = 8;
static const size_t kLayoutFieldCumulativeElevation  = 9;
static const size_t kLayoutFieldConnection           = 11;
static const size_t kLayoutFieldDirection            = 12;
static const size_t kScheduleWayside                 = 7;
static const size_t kScheduleFieldSpeedLimit         = 11;
static const size_t kScheduleFieldTotalTimeToStation = 23;
static const size_t kScheduleFieldTrack              = 0;
static const size_t kLayoutFieldPolarity             = 13;

static void AssignBlockInfrastructure(types::Block &block, const std::string &input_string);
static std::vector<std::string> SplitBySemicolon(const std::string &input);

BlockBuilder::BlockBuilder(const std::vector<std::vector<std::string>> &records, const RecordType record_type)
{
    switch (record_type)
    {
    case RecordType::RECORDTYPE_SCHEDULE:
        BuildBlocksFromSchedule(records);
        break;
    case RecordType::RECORDTYPE_TRACK_LAYOUT:
        BuildBlocksFromTrackLayout(records);
        break;
    default:
        break;
    }
}

types::Error BlockBuilder::GetBlock(const types::BlockId block_id, types::Block &block)
{
    types::Error                        error = types::Error::ERROR_NONE;
    std::vector<types::Block>::iterator it    = std::ranges::find_if(blocks_, [&block_id](const types::Block &block)
    {
        return block.block == block_id;
    });

    // Check if block was found
    if (it != blocks_.end())
    {
        block = *it; // Return the found block
    }
    else
    {
        error = types::Error::ERROR_INVALID_BLOCK;
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

void BlockBuilder::BuildBlocksFromSchedule(const std::vector<std::vector<std::string>> &records)
{
    blocks_.clear();
    blocks_.reserve(records.size());

    for (std::vector<std::vector<std::string>>::const_iterator i = (records.begin() + 1); i != records.end(); ++i)
    {
        blocks_.emplace_back();

        types::Block &                  block  = *(blocks_.end() - 1);
        const std::vector<std::string> &record = *i;

        if (record[kScheduleFieldTrack].find("Green") != std::string::npos)
        {
            block.track = types::TrackId::TRACKID_GREEN;
        }
        else if (record[kScheduleFieldTrack].find("Red") != std::string::npos)
        {
            block.track = types::TrackId::TRACKID_RED;
        }
        else if (record[kScheduleFieldTrack].find("Blue") != std::string::npos)
        {
            block.track = types::TrackId::TRACKID_BLUE;
        }

        block.section     = record[1][0];
        block.block       = static_cast<types::BlockId>(std::stoi(record[kLayoutFieldBlockNumber]));
        block.length      = std::stod(record[kLayoutFieldBlockLength]);
        block.grade       = std::stod(record[kLayoutFieldBlockGrade]);
        block.speed_limit = std::stod(record[kScheduleFieldSpeedLimit]);
        block.wayside     = std::stoi(record[kScheduleWayside]);

        AssignBlockInfrastructure(block, record[kLayoutFieldInfrastructure]);

        if (block.has_station)
        {
            block.total_time_to_station = convert::ConvertMinuteToMilliseconds(record[kScheduleFieldTotalTimeToStation]);
        }
    }
}

void BlockBuilder::BuildBlocksFromTrackLayout(const std::vector<std::vector<std::string>> &records)
{
    blocks_.clear();
    blocks_.reserve(records.size());

    for (std::vector<std::vector<std::string>>::const_iterator i = (records.begin() + 1); i != records.end(); ++i)
    {
        blocks_.emplace_back();

        types::Block &                  block  = *(blocks_.end() - 1);
        const std::vector<std::string> &record = *i;

        block.section              = record[1][0];
        block.block                = static_cast<types::BlockId>(std::stoi(record[kLayoutFieldBlockNumber]));
        block.length               = std::stod(record[kLayoutFieldBlockLength]);
        block.grade                = std::stod(record[kLayoutFieldBlockGrade]);
        block.speed_limit          = std::stod(record[kLayoutFieldSpeedLimit]);
        block.elevation            = std::stod(record[kLayoutFieldElevation]);
        block.cumulative_elevation = std::stod(record[kLayoutFieldCumulativeElevation]);

        AssignBlockInfrastructure(block, record[kLayoutFieldInfrastructure]);

        std::string station_side = record[kLayoutFieldStationSide];
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

        if (record[kLayoutFieldConnection] != "")
        {
            block.switch_connection = static_cast<types::BlockId>(std::stoi(record[kLayoutFieldConnection]));
        }

        std::string block_direction = record[kLayoutFieldDirection];
        if (block_direction.find("UNIDIRECTIONAL") != std::string::npos)
        {
            block.direction = types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL;
        }
        else
        {
            block.direction = types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL;
        }

        if (record[kLayoutFieldPolarity] != "")
        {
            std::string block_polarity = record[kLayoutFieldPolarity];
            if (block_polarity.find("1") != std::string::npos)
            {
                block.polarity = types::Polarity::POLARITY_POSITIVE;
            }
        }
    }
}

static void AssignBlockInfrastructure(types::Block &block, const std::string &input_string)
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
            station_name[0]    = static_cast<char>(std::toupper(station_name[0]));
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

static std::vector<std::string> SplitBySemicolon(const std::string &input)
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