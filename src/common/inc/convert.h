/*****************************************************************************
* @file convert.h
*
* @brief Conversions between commonly used types.
*****************************************************************************/

#include "types.h"

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second);
types::MetersPerSecond MilesPerHourToMetersPerSecond(const types::MilesPerHour miles_per_hour);
types::Feet MetersToFeet(const types::Meters meters);
types::Meters FeetToMeters(const types::Feet feet);
types::Meters MilesToMeters(const types::Miles miles);
types::Miles MetersToMiles(const types::Meters meters);
types::MetersPerSecond KilometersPerHourToMetersPerSecond(const types::KilometersPerHour);
types::KilometersPerHour MetersPerSecondToKilometersPerHour(const types::Meters meters_per_second);

} // namespace convert