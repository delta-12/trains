#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <array>
#include <vector>

#include "convert.h"
#include "display.h"
#include "esp_log.h"
#include "gpio_esp.h"
#include "pin_map.h"
#include "types.h"
#include "uart_esp.h"
#include "wayside_controller.h"

#define DISPLAY_TEXT_BUFFER_SIZE 60
#define MIN_BLOCK_NUMBER 53
#define MAX_BLOCK_NUMBER 127

static const char *kUartEventHandlerTaskName = "UART Event Handler Task";
static char display_text_buffer[DISPLAY_TEXT_BUFFER_SIZE];
static const char *kBlockTextFormatString = "Block: %d\n"
                                            "Commanded Speed: %dmph\n"
                                            "Authority: %dft";
static const char *kBlockTextFormatStringNoCommand = "Block: %d\n"
                                                     "Commanded Speed: --mph\n"
                                                     "Authority: --ft";
static types::BlockId selected_block = MIN_BLOCK_NUMBER; // TODO make thread safe
static int speed = 0;
static int authority = 0;
static bool occupancy = false;
static bool switched_enabled = false;

static std::array<bool, wayside_controller::kTotalInputs> input_values = {false};
static std::array<bool, wayside_controller::kTotalOutputs> output_values = {false};

static const std::vector<wayside_controller::WaysideBlock> kGreenLineBlocks = {
    wayside_controller::WaysideBlock(53, 54, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(54, 55, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 1, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(55, 56, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 2, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(56, 57, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 3, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(57, 58, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 4, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(58, 59, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 5, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(59, 60, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 6, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(60, 61, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 7, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(61, 62, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 8, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(62, 63, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 9, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(63, 64, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 10, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(64, 65, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 11, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(65, 66, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 12, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(66, 67, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 13, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(67, 68, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 14, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(68, 69, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 15, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(69, 70, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(70, 71, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 17, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(71, 72, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 18, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(72, 73, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 19, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(73, 74, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 20, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(74, 75, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 21, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(75, 76, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 22, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(76, 77, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 23, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(77, 78, 101, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 24, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW), // TODO fix how switches are represented
    wayside_controller::WaysideBlock(78, 79, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 25, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(79, 80, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 26, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(80, 81, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 27, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(81, 82, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 28, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(82, 83, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 29, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(83, 84, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 30, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(84, 85, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 31, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(85, 86, 0, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 32, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(86, 87, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 33, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(87, 88, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 34, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(88, 89, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 35, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(89, 90, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 36, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(90, 91, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 37, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(91, 92, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 38, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(92, 93, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 39, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(93, 94, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 40, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(94, 95, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 41, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(95, 96, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 42, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(96, 97, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 43, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(97, 98, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 44, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(98, 99, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 45, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(99, 100, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 46, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(100, 85, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 47, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(101, 102, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 48, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(102, 103, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 49, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(103, 104, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 50, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(104, 105, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 51, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(105, 106, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 52, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(106, 107, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 53, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(107, 108, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 54, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(108, 109, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 55, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(109, 110, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 56, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(110, 111, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 57, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(111, 112, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 58, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(112, 113, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 59, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(113, 114, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 60, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(114, 115, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 61, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(115, 116, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 62, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(116, 117, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 63, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(117, 118, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 64, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(118, 119, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 65, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(119, 120, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 66, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(120, 121, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 67, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(121, 122, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 68, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(122, 123, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 69, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(123, 124, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 70, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(124, 125, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 71, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(125, 126, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 72, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(126, 127, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 73, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(127, 128, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 74, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW)};

static void GpioInit(bsp_esp::EspGpioHandler &handler);
static void ButtonCallback(const bsp::GpioPin pin);
static void DisplayBlock(const types::BlockId block, const types::MetersPerSecond commanded_speed, const types::Meters authority);
void GetInputs(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs);
wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const bool state);

extern "C" void app_main(void)
{
    wayside_controller::WaysideController hardware_wayside_controller(GetInputs, SetOutput, kGreenLineBlocks);

    bsp_esp::EspGpioHandler gpio_handler;
    GpioInit(gpio_handler);

    uart_config_t uart_configuration = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0U,
        .source_clk = UART_SCLK_DEFAULT,
    };
    bsp_esp::EspUartHandler uart_handler(UART_NUM_0, uart_configuration, 1024, 1024, kUartEventHandlerTaskName);
    static char buffer[1024];

    DisplayInit(PIN_MAP_DISPLAY_I2C_SDA, PIN_MAP_DISPLAY_I2C_SCL);
    DisplayBlock(selected_block, -1, -1);
}

static void GpioInit(bsp_esp::EspGpioHandler &handler)
{
    bsp::GpioConfiguration gpio_configuration = {
        .mode = bsp::GPIOMODE_INPUT,
        .bias = bsp::GPIOBIAS_NONE,
        .interrupt = bsp::GPIOINTERRUPT_RISING,
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
        speed = 0;
        authority = 0;
        selected_block--;
        DisplayBlock(selected_block, speed, authority);
        // ESP_LOGI("Selected block", "%d", selected_block);
    }
    else if ((PIN_MAP_RIGHT_BUTTON == pin) && (selected_block < MAX_BLOCK_NUMBER))
    {
        speed = 0;
        authority = 0;
        selected_block++;
        DisplayBlock(selected_block, speed, authority);
        // ESP_LOGI("Selected block", "%d", selected_block);
    }
    else if (PIN_MAP_OCCUPANCY_BUTTON == pin)
    {
        occupancy = !occupancy;
        // ESP_LOGI("Occupancy", "toggle occupancy");
    }
    else if (PIN_MAP_SWITCH_BUTTON == pin && switched_enabled)
    {
        switched = !switched;
        // ESP_LOGI("Switch", "toggled switch to %d", (int)switched);
    }

    if (selected_block == 5)
    {
        switched_enabled = true;
    }
    else
    {
        switched_enabled = false;
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