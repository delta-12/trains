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

typedef double                                      MetersPerSecond;
typedef double                                      MetersPerSecondSquared;
typedef double                                      Meters;
typedef double                                      Watts;
typedef int16_t                                     DegreesFahrenheit;
typedef int64_t                                     Tick;
typedef uint8_t                                     TrackId;
typedef uint16_t                                    BlockId;
typedef uint16_t                                    TrainId;
typedef std::array<uint8_t, TYPES_BEACON_DATA_SIZE> BeaconData;

class Port
{
    public:
        virtual void Send(const std::vector<uint8_t> &buffer) = 0;
        virtual void Receive(std::vector<uint8_t> &buffer)    = 0;
};

} // namespace types

#endif // TRAINS_SRC_COMMON_INC_TYPES_H