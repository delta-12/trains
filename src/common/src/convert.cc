#include "convert.h"

#define CONVERT_METERS_PER_SECOND_TO_MILES_PER_HOUR      (2.23694)
#define CONVERT_MILES_PER_HOUR_TO_METERS_PER_SECOND      (0.44704)
#define CONVERT_METERS_TO_FEET                           (3.28084)
#define CONVERT_FEET_TO_METERS                           (0.3048)
#define CONVERT_MILES_TO_METERS                          (1609.34)
#define CONVERT_METERS_TO_MILES                          (0.000621373)
#define CONVERT_KILOMETERS_PER_HOUR_TO_METERS_PER_SECOND (0.27777)
#define CONVERT_METERS_PER_SECOND_TO_KILOMETERS_PER_HOUR (3.6)
#define MINUTE_TO_SECOND_CONVERSION_FACTOR               (60)
#define SECOND_TO_MILLISECONDS_CONVERSION_FACTOR         (1000)
#define CONVERT_1_BYTE_BIT_SHIFT                         (8)
#define CONVERT_BYTE_MASK                                (0xFF)

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

types::Millisecond ConvertMinuteToMilliseconds(const std::string &minute_string)
{
    return types::Millisecond(static_cast<int64_t>(std::stod(minute_string) * MINUTE_TO_SECOND_CONVERSION_FACTOR * SECOND_TO_MILLISECONDS_CONVERSION_FACTOR));
}

uint8_t GetLowByte(const uint16_t data)
{
    return (data & CONVERT_BYTE_MASK);
}

uint8_t GetHighByte(const uint16_t data)
{
    return ((data >> CONVERT_1_BYTE_BIT_SHIFT) & CONVERT_BYTE_MASK);
}

uint16_t MakeUint16(const uint8_t high, const uint8_t low)
{
    return ((high << CONVERT_1_BYTE_BIT_SHIFT) | low);
}

} // namespace convert