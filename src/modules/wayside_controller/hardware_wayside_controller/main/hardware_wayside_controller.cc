#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_esp.h"

static void ButtonCallback(const bsp::GpioPin pin);

extern "C" void app_main(void)
{
    printf("foobar\n");

    bsp_esp::EspGpioHandler gpio_handler;

    bsp::GpioConfiguration gpio_configuration = {
        .mode = bsp::GPIOMODE_INPUT,
        .bias = bsp::GPIOBIAS_NONE,
        .interrupt = bsp::GPIOINTERRUPT_FALLING,
    };
    gpio_handler.ConfigurePin(23, gpio_configuration);
    gpio_handler.RegisterCallback(23, ButtonCallback);
    gpio_handler.ConfigurePin(32, gpio_configuration);
    gpio_handler.RegisterCallback(32, ButtonCallback);

    gpio_configuration = {
        .mode = bsp::GPIOMODE_OUTPUT,
        .bias = bsp::GPIOBIAS_NONE,
        .interrupt = bsp::GPIOINTERRUPT_DISABLED,
    };
    gpio_handler.ConfigurePin(4, gpio_configuration);
    gpio_handler.ConfigurePin(14, gpio_configuration);
    gpio_handler.ConfigurePin(15, gpio_configuration);
    gpio_handler.ConfigurePin(27, gpio_configuration);
    gpio_handler.ConfigurePin(26, gpio_configuration);
    gpio_handler.ConfigurePin(25, gpio_configuration);

    gpio_handler.SetLevel(4, bsp::GPIOLEVEL_HIGH);
    gpio_handler.SetLevel(14, bsp::GPIOLEVEL_HIGH);
    gpio_handler.SetLevel(15, bsp::GPIOLEVEL_HIGH);
    gpio_handler.SetLevel(27, bsp::GPIOLEVEL_HIGH);
    gpio_handler.SetLevel(26, bsp::GPIOLEVEL_HIGH);
    gpio_handler.SetLevel(25, bsp::GPIOLEVEL_HIGH);

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    gpio_handler.SetLevel(4, bsp::GPIOLEVEL_LOW);
    gpio_handler.SetLevel(14, bsp::GPIOLEVEL_LOW);
    gpio_handler.SetLevel(15, bsp::GPIOLEVEL_LOW);
    gpio_handler.SetLevel(27, bsp::GPIOLEVEL_LOW);
    gpio_handler.SetLevel(26, bsp::GPIOLEVEL_LOW);
    gpio_handler.SetLevel(25, bsp::GPIOLEVEL_LOW);
}

static void ButtonCallback(const bsp::GpioPin pin)
{
    if (23 == pin)
    {
        printf("Left button\n");
    }
    else if (32 == pin)
    {
        printf("Right button\n");
    }

}