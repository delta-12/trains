/*****************************************************************************
* @file convert.h
*
* @brief Conversions between commonly used types.
*****************************************************************************/

#include "types.h"

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second);
types::MetersPerSecond MilesPerHourToMilesPerHour(const types::MetersPerSecond meters_per_second);
types::Feet MetersToFeet(const types::Meters meters);
types::Meters FeetToMeters(const types::Feet feet);
types::MetersPerSecond KilometersPerHourToMetersPerSecond(const types::KilometersPerHour);
} // namespace convert