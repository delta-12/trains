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
    ERROR_INVALID_SIZE
} Error;

typedef enum
{
    POLARITY_POSITIVE,
    POLARITY_NEGATIVE
} Polarity;

typedef uint8_t                                     MetersPerSecond;
typedef uint32_t                                    Meters;
typedef uint8_t                                     MilesPerHour;
typedef uint32_t                                    Miles;
typedef uint32_t                                    Watts;
typedef int16_t                                     DegreesFahrenheit;
typedef uint8_t                                     TrackId;
typedef uint16_t                                    BlockId;
typedef uint16_t                                    TrainId;
typedef std::array<uint8_t, TYPES_BEACON_DATA_SIZE> BeaconData;

class Port
{
    public:
        virtual void Send(const std::vector<uint8_t> &buffer) = 0;
        virtual void Receive(std::vector<uint8_t> &buffer)    = 0;

    private:
        void testprive();
};

} // namespace types

#endif // TRAINS_SRC_COMMON_INC_TYPES_H