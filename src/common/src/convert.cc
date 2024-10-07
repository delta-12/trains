#include "convert.h"

#define CONVERT_METERS_PER_SECOND_TO_MILES_PER_HOUR (2.23694)
#define CONVERT_METERS_TO_FEET                      (3.28084)

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second)
{
    return meters_per_second * CONVERT_METERS_PER_SECOND_TO_MILES_PER_HOUR;
}

types::Feet MetersToFeet(const types::Meters meters)
{
    return meters * CONVERT_METERS_TO_FEET;
}

} // namespace convert