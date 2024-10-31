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

#define TYPES_BEACON_DATA_SIZE 128U
#define TYPES_END_BLOCK_ID     0U

namespace types
{

typedef double                                      KilometersPerHour;
typedef double                                      Kilometers;
typedef double                                      MetersPerSecond;
typedef double                                      MetersPerSecondSquared;
typedef double                                      Meters;
typedef double                                      MilesPerHour;
typedef double                                      Miles;
typedef double                                      Feet;
typedef double                                      Watts;
typedef int16_t                                     DegreesFahrenheit;
typedef int64_t                                     Tick;
typedef char                                        SectionId;
typedef uint16_t                                    BlockId;
typedef uint16_t                                    Blocks;
typedef uint16_t                                    ControllerId;
typedef ControllerId                                TrainId;
typedef ControllerId                                WaysideId;
typedef std::array<uint8_t, TYPES_BEACON_DATA_SIZE> BeaconData;
typedef struct Block                                Block;
typedef struct TrackCircuitData                     TrackCircuitData;
typedef struct BlockState                           BlockState;
typedef std::chrono::duration<double>               Second;

typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_TRACK,
    ERROR_DUPLICATE_TRACK,
    ERROR_INVALID_BLOCK,
    ERROR_INVALID_TRAIN,
    ERROR_INVALID_MODE,
    ERROR_INVALID_SIZE,
    ERROR_INVALID_FORMAT,
    ERROR_IO
} Error;

typedef enum
{
    POLARITY_POSITIVE,
    POLARITY_NEGATIVE
} Polarity;

typedef enum
{
    TRAFFICLIGHTCOLOR_RED,
    TRAFFICLIGHTCOLOR_GREEN
} TrafficLightColor;

typedef enum
{
    BLOCKDIRECTION_UNIDIRECTIONAL,
    BLOCKDIRECTION_BIDIRECTIONAL
} BlockDirection;

typedef enum
{
    TRACKID_NONE,
    TRACKID_RED,
    TRACKID_GREEN,
} TrackId;

typedef enum
{
    STATIONSIDE_LEFT,
    STATIONSIDE_RIGHT,
    STATIONSIDE_BOTH
} StationSide;

struct Block
{
    Block() : block(0), occupied(false), has_switch(false), switched(false), switch_connection(0), has_crossing(false), crossing_state(false), has_light(false),
              power_failure(false), track_circuit_failure(false), broken_rail(false), grade(0), length(0), speed_limit(0), elevation(0), cumulative_elevation(0),
              section('\0'), heater_on(false), has_station(false), underground(false)
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
};

struct TrackCircuitData
{
    public:
        TrackCircuitData(void) : track(TRACKID_RED), block(0), speed(0.0), authority(0)
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