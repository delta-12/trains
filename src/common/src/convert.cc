#include "convert.h"

#define CONVERT_METERS_PER_SECOND_TO_MILES_PER_HOUR         (2.23694)
#define CONVERT_MILES_PER_HOUR_TO_METERS_PER_SECOND         (0.44704)
#define CONVERT_METERS_TO_FEET                              (3.28084)
#define CONVERT_FEET_TO_METERS                              (0.3048)
#define CONVERT_MILES_TO_METERS                             (1609.34)
#define CONVERT_METERS_TO_MILES                             (0.000621373)
#define CONVERT_KILOMETERS_PER_HOUR_TO_METERS_PER_SECOND    (0.27777)
#define CONVERT_METERS_PER_SECOND_TO_KILOMETERS_PER_HOUR    (3.6)

namespace convert
{

types::MilesPerHour MetersPerSecondToMilesPerHour(const types::MetersPerSecond meters_per_second)
{
    return meters_per_second * CONVERT_METERS_PER_SECOND_TO_MILES_PER_HOUR;
}

types::MetersPerSecond MilesPerHourToMetersPerSecond(const types::MilesPerHour miles_per_hour)
{
    return miles_per_hour * CONVERT_MILES_PER_HOUR_TO_METERS_PER_SECOND;
}

types::Feet MetersToFeet(const types::Meters meters)
{
    return meters * CONVERT_METERS_TO_FEET;
}

types::Meters FeetToMeters(const types::Feet feet) 
{
    return feet * CONVERT_FEET_TO_METERS;
}

types::Meters MilesToMeters(const types::Miles miles) 
{
    return miles * CONVERT_MILES_TO_METERS;
}

types::Miles MetersToMiles(const types::Meters meters) 
{
    return meters * CONVERT_METERS_TO_MILES;
}

types::Meters KilometersPerHourToMetersPerSecond(const types::KilometersPerHour kilometers_per_hour)
{
    return kilometers_per_hour * CONVERT_KILOMETERS_PER_HOUR_TO_METERS_PER_SECOND;
}

types::KilometersPerHour MetersPerSecondToKilometersPerHour(const types::Meters meters_per_second) 
{
    return meters_per_second * CONVERT_METERS_PER_SECOND_TO_KILOMETERS_PER_HOUR;
}

} // namespace convert