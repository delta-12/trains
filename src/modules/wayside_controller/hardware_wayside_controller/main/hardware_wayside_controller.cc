#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "convert.h"
#include "display.h"
#include "gpio_esp.h"
#include "types.h"

#define DISPLAY_TEXT_BUFFER_SIZE 60
#define LEFT_BUTTON 23
#define RIGHT_BUTTON 32
#define MIN_BLOCK_NUMBER 1
#define MAX_BLOCK_NUMBER 16

static char display_text_buffer[DISPLAY_TEXT_BUFFER_SIZE];
static const char *block_text_format_string = "Block: %d\n"
                                              "Commanded Speed: %dmph\n"
                                              "Authority: %dft";
static const char *block_text_format_string_no_command = "Block: %d\n"
                                                         "Commanded Speed: --mph\n"
                                                         "Authority: --ft";
static types::BlockId selected_block = MIN_BLOCK_NUMBER;

static void ButtonCallback(const bsp::GpioPin pin);
static void DisplayBlock(const types::BlockId block, const types::MetersPerSecond commanded_speed, const types::Meters authority);

extern "C" void app_main(void)
{
    printf("foobar\n");

    DisplayInit();
    DisplayBlock(selected_block, 20, 150);

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

    DisplayBlock(selected_block, 15, -10);
}

static void ButtonCallback(const bsp::GpioPin pin)
{
    if ((LEFT_BUTTON == pin) && (selected_block > MIN_BLOCK_NUMBER))
    {
        printf("Left button\n");
        selected_block--;
    }
    if ((RIGHT_BUTTON == pin) && (selected_block < MAX_BLOCK_NUMBER))
    {
        printf("Right button\n");
        selected_block++;
    }
}

static void DisplayBlock(const types::BlockId block, const types::MetersPerSecond commanded_speed, const types::Meters authority)
{
    uint32_t converted_speed = (uint32_t)convert::MetersPerSecondToMilesPerHour(commanded_speed);
    uint32_t converted_authority = (uint32_t)convert::MetersToFeet(authority);

    if (commanded_speed < 0)
    {
        converted_speed = 0;
    }
    if (authority < 0)
    {
        converted_authority = 0;
    }

    if ((commanded_speed > 0.0) || (authority > 0.0))
    {
        snprintf(display_text_buffer, DISPLAY_TEXT_BUFFER_SIZE, block_text_format_string, block, converted_speed, converted_authority);
    }
    else
    {
        snprintf(display_text_buffer, DISPLAY_TEXT_BUFFER_SIZE, block_text_format_string_no_command, block);
    }

    DisplayWriteString(display_text_buffer);
}