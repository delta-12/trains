/*****************************************************************************
* @file types.h
*
* @brief Defines common types for the project.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_TYPES_H
#define TRAINS_SRC_COMMON_INC_TYPES_H

#include <array>
#include <cstdint>
#include <vector>

#define TYPES_BEACON_DATA_SIZE 128U

namespace types
{

typedef double                                      MetersPerSecond;
typedef double                                      MetersPerSecondSquared;
typedef double                                      Meters;
typedef double                                      Watts;
typedef int16_t                                     DegreesFahrenheit;
typedef int64_t                                     Tick;
typedef char                                        SectionId;
typedef uint16_t                                    BlockId;
typedef uint16_t                                    TrainId;
typedef std::array<uint8_t, TYPES_BEACON_DATA_SIZE> BeaconData;

typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_TRACK,
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
    LIGHTCOLOR_RED,
    LIGHTCOLOR_GREEN,
    LIGHTCOLOR_NONE
} LightColor;

typedef enum
{
    BLOCKDIRECTION_UNIDIRECTIONAL,
    BLOCKDIRECTION_BIDIRECTIONAL
} BlockDirection;

typedef enum
{
    TRACKID_RED,
    TRACKID_GREEN
} TrackId;

typedef struct
{
    BlockId block;
    bool occupied;
    bool has_switch;
    bool switched;
    BlockId switch_connection;
    bool has_crossing;
    bool crossing_state;
    bool has_light;
    LightColor light_color;
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
    BlockDirection direction;
    bool underground;
} Block;

class Port
{
    public:
        virtual void Send(const std::vector<uint8_t> &buffer) = 0;
        virtual void Receive(std::vector<uint8_t> &buffer)    = 0;
};

} // namespace types

#endif // TRAINS_SRC_COMMON_INC_TYPES_H