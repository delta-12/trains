/*****************************************************************************
* @file gpio.h
*
* @brief Implements the BSP GPIO module for the ESP32.
*****************************************************************************/
#ifndef TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_GPIO_ESP_H
#define TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_GPIO_ESP_H

#include "driver/gpio.h"

#include "gpio.h"

namespace bsp_esp
{

// TODO make class static or a singleton?
class EspGpioHandler : public bsp::GpioHandler
{
    public:
        void ConfigurePin(const bsp::GpioPin pin, const bsp::GpioConfiguration configuration);
        void ConfigurePins(const bsp::GpioPinMask mask, const bsp::GpioConfiguration configuration);
        void RegisterCallback(const bsp::GpioPin pin, std::function<void(const bsp::GpioPin pin)> callback);
        void SetLevel(const bsp::GpioPin pin, const bsp::GpioLevel level);
        bsp::GpioLevel GetLevel(const bsp::GpioPin pin);       
};

} // namespace bsp_esp

#endif // TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_GPIO_ESP_H