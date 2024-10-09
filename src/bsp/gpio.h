/*****************************************************************************
* @file gpio.h
*
* @brief Defines BSP interface for manipulating GPIO.
*****************************************************************************/
#ifndef TRAINS_SRC_BSP_GPIO_H
#define TRAINS_SRC_BSP_GPIO_H

#include <cstdint>
#include <functional>

namespace bsp
{

typedef uint32_t GpioPin;
typedef uint64_t GpioPinMask;

typedef enum
{
    GPIOLEVEL_LOW,
    GPIOLEVEL_HIGH
} GpioLevel;

typedef enum
{
    GPIOMODE_INPUT,
    GPIOMODE_OUTPUT
} GpioMode;

typedef enum
{
    GPIOBIAS_NONE,
    GPIOBIAS_PULLUP,
    GPIOBIAS_PULLDOWN,
    GPIOBIAS_DEFAULT
} GpioBias;

typedef enum
{
    GPIOINTERRUPT_DISABLED,
    GPIOINTERRUPT_RISING,
    GPIOINTERRUPT_FALLING,
    GPIOINTERRUPT_CHANGE
} GpioInterrupt;

typedef struct
{
    public:
        GpioMode mode;
        GpioBias bias;
        GpioInterrupt interrupt;
} GpioConfiguration;

class GpioHandler
{
    public:
        virtual void ConfigurePin(const GpioPin pin, const GpioConfiguration configuration)               = 0;
        virtual void ConfigurePins(const GpioPinMask mask, const GpioConfiguration configuration)         = 0;
        virtual void RegisterCallback(const GpioPin pin, std::function<void(const GpioPin pin)> callback) = 0;
        virtual void SetLevel(const GpioPin pin, const GpioLevel level)                                   = 0;
        virtual GpioLevel GetLevel(const GpioPin pin)                                                     = 0;
};

} // namespace bsp

#endif // TRAINS_SRC_BSP_GPIO_H