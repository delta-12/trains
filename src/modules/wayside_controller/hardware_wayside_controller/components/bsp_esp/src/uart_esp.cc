#include "uart_esp.h"

#include "freertos/task.h"

#define ESP_UART_TASK_STACK_DEPTH 2048U
#define ESP_UART_TASK_PRIORITY 10U
#define ESP_UART_QUEUE_SIZE 20U
#define ESP_UART_INTERRUPT_FLAG_DEFAULT 0U

static void EspUartEventHandlerTask(void *arg);

namespace bsp_esp
{

EspUartHandler::EspUartHandler(const uart_port_t uart_port, const uart_config_t &uart_config, const size_t rx_buffer_size, const size_t tx_buffer_size, const char *const task_name) : uart_port_(uart_port)
{
    ESP_ERROR_CHECK(uart_param_config(uart_port_, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_port_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_port_, rx_buffer_size, tx_buffer_size, ESP_UART_QUEUE_SIZE, &uart_queue_handle_, ESP_UART_INTERRUPT_FLAG_DEFAULT));

    xTaskCreate(EspUartEventHandlerTask, task_name, ESP_UART_TASK_STACK_DEPTH, uart_queue_handle_, ESP_UART_TASK_PRIORITY, NULL);
}

EspUartHandler::EspUartHandler(const uart_port_t uart_port, const uart_config_t &uart_config, const size_t rx_buffer_size, const size_t tx_buffer_size, const bsp::GpioPin rx_pin, const bsp::GpioPin tx_pin, const char *const task_name) : uart_port_(uart_port)
{
    ESP_ERROR_CHECK(uart_param_config(uart_port_, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_port_, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_port_, rx_buffer_size, tx_buffer_size, ESP_UART_QUEUE_SIZE, &uart_queue_handle_, ESP_UART_INTERRUPT_FLAG_DEFAULT));

    xTaskCreate(EspUartEventHandlerTask, task_name, ESP_UART_TASK_STACK_DEPTH, uart_queue_handle_, ESP_UART_TASK_PRIORITY, NULL);
}

EspUartHandler::~EspUartHandler(void)
{
    if (uart_is_driver_installed(uart_port_))
    {
        ESP_ERROR_CHECK(uart_driver_delete(uart_port_));
    }
}

bool EspUartHandler::SetBaudRate(const bsp::BaudRate baud_rate)
{
    bool baud_rate_set = false;

    if (ESP_OK == uart_set_baudrate(uart_port_, baud_rate))
    {
        baud_rate_set = true;
    }

    return baud_rate_set;
}

bsp::BaudRate EspUartHandler::GetBaudRate(void) const
{
    bsp::BaudRate baud_rate = 0U;

    ESP_ERROR_CHECK(uart_get_baudrate(uart_port_, &baud_rate));

    return baud_rate;
}

bool EspUartHandler::SetPartiy(const bsp::Parity parity)
{
    bool parity_set = false;

    if (ESP_OK == uart_set_parity(uart_port_, static_cast<uart_parity_t>(parity)))
    {
        parity_set = true;
    }

    return parity_set;
}

bsp::Parity EspUartHandler::GetParity(void) const
{
    bsp::Parity parity = bsp::PARITY_NONE;

    ESP_ERROR_CHECK(uart_get_parity(uart_port_, (uart_parity_t *)&parity));

    return parity;
}

size_t EspUartHandler::BytesAvailable(void) const
{
    size_t bytes_available = 0U;

    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_port_, static_cast<size_t *>(&bytes_available)));

    return bytes_available;
}

uint8_t EspUartHandler::Read(void)
{
    uint8_t byte_read = 0U;

    Read(&byte_read, 1U);

    return byte_read;
}

size_t EspUartHandler::Read(uint8_t *const buffer, const size_t size)
{
    int64_t bytes_read = 0U;
    size_t buffered_data_size = BytesAvailable();

    if (size > buffered_data_size)
    {
        bytes_read = uart_read_bytes(uart_port_, buffer, buffered_data_size, portMAX_DELAY);
    }
    else
    {
        bytes_read = uart_read_bytes(uart_port_, buffer, size, portMAX_DELAY);
    }

    if (bytes_read < 0U)
    {
        bytes_read = 0U;
    }

    return static_cast<size_t>(bytes_read);
}

size_t EspUartHandler::Write(const uint8_t byte)
{
    return Write(&byte, 1U);
}

size_t EspUartHandler::Write(const uint8_t *const buffer, const size_t size)
{
    return uart_write_bytes(uart_port_, buffer, size);
}

uart_port_t EspUartHandler::GetUartPort(void) const
{
    return uart_port_;
}

QueueHandle_t EspUartHandler::GetUartQueueHandle(void) const
{
    return uart_queue_handle_;
}

} // namespace bsp_esp

static void EspUartEventHandlerTask(void *arg)
{
    bsp_esp::EspUartHandler *uart_handler = static_cast<bsp_esp::EspUartHandler *>(arg);
    QueueHandle_t uart_queue_handle = uart_handler->GetUartQueueHandle();
    uart_port_t uart_port = uart_handler->GetUartPort();
    uart_event_t event;

    for (;;)
    {
        // TODO does not work with UART 0, need to fix
        // if (xQueueReceive(uart_queue_handle, static_cast<void *>(&event), (TickType_t)portMAX_DELAY))
        // {
        //     switch (event.type)
        //     {
        //     case UART_FIFO_OVF:
        //         uart_flush_input(uart_port);
        //         xQueueReset(uart_queue_handle);
        //         break;
        //     case UART_BUFFER_FULL:
        //         uart_flush_input(uart_port);
        //         xQueueReset(uart_queue_handle);
        //         break;
        //     default:
        //         break;
        //     }
        // }

        // TODO fix
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}