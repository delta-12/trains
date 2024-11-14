/*****************************************************************************
* @file uart_esp.h
*
* @brief Implements the BSP UART module for the ESP32.
*****************************************************************************/
#ifndef TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_UART_ESP_H
#define TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_UART_ESP_H

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "gpio.h"
#include "uart.h"

namespace bsp_esp
{

class EspUartHandler : public bsp::UartHandler
{
    public:
        EspUartHandler(const uart_port_t uart_port, const uart_config_t &uart_config, const size_t rx_buffer_size, const size_t tx_buffer_size, const char *const task_name);
        EspUartHandler(const uart_port_t uart_port, const uart_config_t &uart_config, const size_t rx_buffer_size, const size_t tx_buffer_size, const bsp::GpioPin rx_pin, const bsp::GpioPin tx_pin, const char *const task_name);
        ~EspUartHandler(void);
        bool SetBaudRate(const bsp::BaudRate baud_rate);
        bsp::BaudRate GetBaudRate(void) const;
        bool SetPartiy(const bsp::Parity parity);
        bsp::Parity GetParity(void) const;
        size_t BytesAvailable(void) const;
        uint8_t Read(void);
        size_t Read(uint8_t *const buffer, const size_t size);
        size_t Write(const uint8_t byte);
        size_t Write(const uint8_t *const buffer, const size_t size);
        uart_port_t GetUartPort(void) const;
        QueueHandle_t GetUartQueueHandle(void) const;

    private:
        uart_port_t uart_port_;
        QueueHandle_t uart_queue_handle_;
};

} // namespace bsp_esp

#endif // TRAINS_HARDWARE_WAYSIDE_CONTROLLER_BSP_ESP_UART_ESP_H