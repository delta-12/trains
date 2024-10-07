/*****************************************************************************
* @file convert.h
*
* @brief Conversions between commonly used types.
*****************************************************************************/

#include "types.h"

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second);
types::Feet MetersToFeet(const types::Meters meters);

} // namespace convert