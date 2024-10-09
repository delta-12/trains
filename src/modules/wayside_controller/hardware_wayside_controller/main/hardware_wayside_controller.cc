#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <array>
#include <vector>

#include "convert.h"
#include "display.h"
#include "gpio_esp.h"
#include "pin_map.h"
#include "types.h"
#include "wayside_controller.h"

#define DISPLAY_TEXT_BUFFER_SIZE 60
#define MIN_BLOCK_NUMBER 1
#define MAX_BLOCK_NUMBER 16

static char display_text_buffer[DISPLAY_TEXT_BUFFER_SIZE];
static const char *kBlockTextFormatString = "Block: %d\n"
                                            "Commanded Speed: %dmph\n"
                                            "Authority: %dft";
static const char *kBlockTextFormatStringNoCommand = "Block: %d\n"
                                                     "Commanded Speed: --mph\n"
                                                     "Authority: --ft";
static types::BlockId selected_block = MIN_BLOCK_NUMBER;  // TODO make thread safe
static std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> input_values = {false};
static std::array<bool, WAYSIDE_CONTROLLER_TOTAL_OUTPUTS> output_values = {false};
static const std::vector<wayside_controller::BlockInputs> kBlueLineBlockInputs = {
    wayside_controller::BlockInputs(1, 0, 0, false, false),
    wayside_controller::BlockInputs(2, 1, 0, false, false),
    wayside_controller::BlockInputs(3, 2, 0, false, false),
    wayside_controller::BlockInputs(4, 3, 0, false, false),
    wayside_controller::BlockInputs(5, 4, 75, true, false),
    wayside_controller::BlockInputs(6, 5, 0, false, false),
    wayside_controller::BlockInputs(7, 6, 0, false, false),
    wayside_controller::BlockInputs(8, 7, 0, false, false),
    wayside_controller::BlockInputs(9, 8, 0, false, false),
    wayside_controller::BlockInputs(10, 9, 0, false, false),
    wayside_controller::BlockInputs(11, 10, 0, false, false),
    wayside_controller::BlockInputs(12, 11, 0, false, false),
    wayside_controller::BlockInputs(13, 12, 0, false, false),
    wayside_controller::BlockInputs(14, 13, 0, false, false),
    wayside_controller::BlockInputs(15, 14, 0, false, false),
    wayside_controller::BlockInputs(16, 15, 0, false, false)};

static void GpioInit(bsp_esp::EspGpioHandler &handler);
static void ButtonCallback(const bsp::GpioPin pin);
static void DisplayBlock(const types::BlockId block, const types::MetersPerSecond commanded_speed, const types::Meters authority);
void GetInputs(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs);
wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const bool state);

extern "C" void app_main(void)
{
    wayside_controller::WaysideController hardware_wayside_controller(GetInputs, SetOutput, kBlueLineBlockInputs);

    bsp_esp::EspGpioHandler gpio_handler;
    DisplayInit(PIN_MAP_DISPLAY_I2C_SDA, PIN_MAP_DISPLAY_I2C_SCL);
    GpioInit(gpio_handler);

    DisplayBlock(selected_block, -1, -1);
}

static void GpioInit(bsp_esp::EspGpioHandler &handler)
{
    bsp::GpioConfiguration gpio_configuration = {
        .mode = bsp::GPIOMODE_INPUT,
        .bias = bsp::GPIOBIAS_NONE,
        .interrupt = bsp::GPIOINTERRUPT_FALLING,
    };
    handler.ConfigurePins(PIN_MAP_BUTTON_MASK, gpio_configuration);
    handler.RegisterCallback(PIN_MAP_LEFT_BUTTON, ButtonCallback);
    handler.RegisterCallback(PIN_MAP_RIGHT_BUTTON, ButtonCallback);
    handler.RegisterCallback(PIN_MAP_OCCUPANCY_BUTTON, ButtonCallback);
    handler.RegisterCallback(PIN_MAP_SWITCH_BUTTON, ButtonCallback);

    gpio_configuration = {
        .mode = bsp::GPIOMODE_OUTPUT,
        .bias = bsp::GPIOBIAS_NONE,
        .interrupt = bsp::GPIOINTERRUPT_DISABLED,
    };
    handler.ConfigurePins(PIN_MAP_LED_MASK, gpio_configuration);

    handler.SetLevel(PIN_MAP_OCCUPANCY_LED, bsp::GPIOLEVEL_LOW);
    handler.SetLevel(PIN_MAP_SWITCH_0_LED, bsp::GPIOLEVEL_LOW);
    handler.SetLevel(PIN_MAP_SWITCH_1_LED, bsp::GPIOLEVEL_LOW);
    handler.SetLevel(PIN_MAP_RED_TRAFFIC_SIGNAL_LED, bsp::GPIOLEVEL_LOW);
    handler.SetLevel(PIN_MAP_GREEN_TRAFFIC_SIGNAL_LED, bsp::GPIOLEVEL_LOW);
    handler.SetLevel(PIN_MAP_CROSSING_LED, bsp::GPIOLEVEL_LOW);
}

static void ButtonCallback(const bsp::GpioPin pin)
{
    if ((PIN_MAP_LEFT_BUTTON == pin) && (selected_block > MIN_BLOCK_NUMBER))
    {
        printf("Left button\n");
        selected_block--;
    }
    if ((PIN_MAP_RIGHT_BUTTON == pin) && (selected_block < MAX_BLOCK_NUMBER))
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
        snprintf(display_text_buffer, DISPLAY_TEXT_BUFFER_SIZE, kBlockTextFormatString, block, converted_speed, converted_authority);
    }
    else
    {
        snprintf(display_text_buffer, DISPLAY_TEXT_BUFFER_SIZE, kBlockTextFormatStringNoCommand, block);
    }

    DisplayWriteString(display_text_buffer);
}

void GetInputs(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)
{
    inputs = input_values;
}

wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const bool state)
{
    wayside_controller::Error error = wayside_controller::ERROR_INVALID_OUTPUT;

    if (output < output_values.size())
    {
        output_values[output] = state;

        error = wayside_controller::ERROR_NONE;
    }

    return error;
}
