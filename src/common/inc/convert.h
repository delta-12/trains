/*****************************************************************************
* @file convert.h
*
* @brief Conversions between commonly used types.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CONVERT_H
#define TRAINS_SRC_COMMON_INC_CONVERT_H

#include "types.h"

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second);
types::MetersPerSecond MilesPerHourToMetersPerSecond(const types::MilesPerHour miles_per_hour);
types::Feet MetersToFeet(const types::Meters meters);
types::Meters FeetToMeters(const types::Feet feet);
types::Meters MilesToMeters(const types::Miles miles);
types::Miles MetersToMiles(const types::Meters meters);
types::MetersPerSecond KilometersPerHourToMetersPerSecond(const types::KilometersPerHour kilometers_per_hour);
types::KilometersPerHour MetersPerSecondToKilometersPerHour(const types::Meters meters_per_second);
types::Millisecond ConvertMinuteToMilliseconds(const std::string &minute_string);
uint8_t GetLowByte(const uint16_t data);
uint8_t GetHighByte(const uint16_t data);
uint16_t MakeUint16(const uint8_t high, const uint8_t low);

} // namespace convert

#endif // TRAINS_SRC_COMMON_INC_CONVERT_H