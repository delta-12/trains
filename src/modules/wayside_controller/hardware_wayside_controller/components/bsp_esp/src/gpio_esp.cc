#include "gpio_esp.h"

namespace bsp_esp
{

void EspGpioHandler::ConfigurePin(const bsp::GpioPin pin, const bsp::GpioConfiguration configuration)
{
    // TODO
}

void EspGpioHandler::RegisterCallback(const bsp::GpioPin pin, std::function<void(const bsp::GpioPin pin)> callback)
{
    // TODO
}

void EspGpioHandler::SetLevel(const bsp::GpioPin pin, const bsp::GpioLevel level)
{
    // TODO
}

bsp::GpioLevel EspGpioHandler::GetLevel(const bsp::GpioPin pin)
{
    // TODO

    return bsp::GPIOLEVEL_LOW;
}

} // namespace bsp_esp