/*****************************************************************************
* @file types.h
*
* @brief Defines common types for the project.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_TYPES_H
#define TRAINS_SRC_COMMON_INC_TYPES_H

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace types
{

static const size_t kBeaconSize = 128;

using KilometersPerHour      = double;
using Kilometers             = double;
using MetersPerSecond        = double;
using MetersPerSecondSquared = double;
using Meters                 = double;
using MilesPerHour           = double;
using Miles                  = double;
using Feet                   = double;
using Watts                  = double;
using DegreesFahrenheit      = int16_t;
using Tick                   = int64_t;
using SectionId              = char;
using BlockId                = uint32_t;
using Blocks                 = BlockId;
using ControllerId           = uint32_t;
using TrainId                = ControllerId;
using WaysideId              = ControllerId;
using BeaconData             = std::array<uint8_t, kBeaconSize>;
using Block                  = struct Block;
using TrackCircuitData       = struct TrackCircuitData;
using BlockState             = struct BlockState;
using Second                 = std::chrono::duration<double>;
using Millisecond            = std::chrono::milliseconds;

static const BlockId kYardBlock = 0;
static const BlockId kEndBlock  = UINT32_MAX;

enum class Error
{
    ERROR_NONE,
    ERROR_INVALID_TRACK,
    ERROR_DUPLICATE_TRACK,
    ERROR_INVALID_BLOCK,
    ERROR_INVALID_TRAIN,
    ERROR_INVALID_MODE,
    ERROR_INVALID_SIZE,
    ERROR_INVALID_FORMAT,
    ERROR_INVALID_CONTROLLER,
    ERROR_IO
};

enum class Polarity
{
    POLARITY_POSITIVE,
    POLARITY_NEGATIVE
};

enum class TrafficLightColor
{
    TRAFFICLIGHTCOLOR_NONE,
    TRAFFICLIGHTCOLOR_RED,
    TRAFFICLIGHTCOLOR_GREEN
};

enum class BlockDirection
{
    BLOCKDIRECTION_UNIDIRECTIONAL,
    BLOCKDIRECTION_BIDIRECTIONAL
};

enum class TrackId
{
    TRACKID_RED,
    TRACKID_GREEN,
    TRACKID_BLUE,
    TRACKID_MAX
};

enum class StationSide
{
    STATIONSIDE_LEFT,
    STATIONSIDE_RIGHT,
    STATIONSIDE_BOTH
};

struct Block
{
    Block() : block(0), occupied(false), has_switch(false), switched(false), switch_connection(0), has_crossing(false), crossing_state(false), has_light(false),
              light_color(TrafficLightColor::TRAFFICLIGHTCOLOR_NONE), power_failure(false), track_circuit_failure(false), broken_rail(false), grade(0), length(0),
              speed_limit(0), elevation(0), cumulative_elevation(0), section('\0'), heater_on(false), has_station(false), station_side(StationSide::STATIONSIDE_LEFT),
              direction(BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL), underground(false), wayside(0), polarity(types::Polarity::POLARITY_NEGATIVE), track(TrackId::TRACKID_RED),
              maintenance(false)
    {
    };
    BlockId block;
    bool occupied;
    bool has_switch;
    bool switched;
    BlockId switch_connection;
    bool has_crossing;
    bool crossing_state;
    bool has_light;
    TrafficLightColor light_color;
    bool power_failure;
    bool track_circuit_failure;
    bool broken_rail;
    bool failed;
    double grade;
    Meters length;
    MetersPerSecond speed_limit;
    Meters elevation;
    Meters cumulative_elevation;
    SectionId section;
    bool heater_on;
    bool has_station;
    std::string station_name;
    StationSide station_side;
    BlockDirection direction;
    bool underground;
    Second total_time_to_station;
    WaysideId wayside;
    types::Polarity polarity;
    TrackId track;
    bool maintenance;
};

struct TrackCircuitData
{
    public:
        TrackCircuitData(void) : track(TrackId::TRACKID_RED), block(0), speed(0.0), authority(0)
        {
        }
        TrackCircuitData(const TrackId track, const BlockId block, const MetersPerSecond speed, const size_t authority) : track(track), block(block), speed(speed), authority(authority)
        {
        }
        TrackId track;
        BlockId block;
        MetersPerSecond speed;
        size_t authority;
};

struct BlockState
{
    public:
        BlockState(void) : block(0), occupied(false), track_failure(false)
        {
        }
        BlockState(const BlockId block, const bool occupied, const bool track_failure) : block(block), occupied(occupied), track_failure(track_failure)
        {
        }
        BlockId block;
        bool occupied;
        bool track_failure;
};

class Port
{
    public:
        virtual size_t Send(const uint8_t *const buffer, const size_t size) = 0;
        virtual size_t SendAvailable(void)                                  = 0;
        virtual size_t Receive(uint8_t *const buffer, const size_t size)    = 0;
        virtual size_t ReceiveAvailable(void)                               = 0;
        virtual bool Connected(void)                                        = 0;
};

} // namespace types

#endif // TRAINS_SRC_COMMON_INC_TYPES_H