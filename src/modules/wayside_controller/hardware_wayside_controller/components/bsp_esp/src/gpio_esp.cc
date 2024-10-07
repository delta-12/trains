#include "gpio_esp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <unordered_map>

#define GPIO_ESP_BIT 1ULL

#define GPIO_ESP_EVENT_QUEUE_SIZE 20U
#define GPIO_ESP_INTR_FLAG_DEFAULT 0U
#define GPIO_ESP_TASK_STACK_DEPTH 2048U
#define GPIO_ESP_TASK_PRIORITY 10U
#define GPIO_ESP_TASK_NAME "GpioEspEventHandlerTask"

static bool isr_service_installed = false; // Flag set if ISR service was installed

static StaticQueue_t event_queue;                                                              // Queue for storing GPIO interrupt events
static uint8_t event_queue_buffer[GPIO_ESP_EVENT_QUEUE_SIZE * sizeof(bsp::GpioPin)];           // Statically allocated buffer for GPIO interrupt event queue's storage area
static QueueHandle_t event_queue_handle = NULL;                                                // Handle for queue storing GPIO interrupt events
// TODO make thread-safe, replace with client-provided lookup function?
std::unordered_map<bsp::GpioPin, std::function<void(const bsp::GpioPin pin)>> event_handlers;  // Map of registered event handlers

static void GpioEspEventHandlerTask(void *arg);
static void GpioEspIsrHandler(void *arg);
static gpio_mode_t ConvertGpioMode(const bsp::GpioMode mode);
static gpio_int_type_t ConvertGpioInterrupt(const bsp::GpioInterrupt interrupt);

namespace bsp_esp
{

void EspGpioHandler::ConfigurePin(const bsp::GpioPin pin, const bsp::GpioConfiguration configuration)
{
    gpio_config_t esp_gpio_config = {
        .pin_bit_mask = (GPIO_ESP_BIT << pin),
        .mode = ConvertGpioMode(configuration.mode),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = ConvertGpioInterrupt(configuration.interrupt),
    };

    if (bsp::GPIOBIAS_PULLUP == configuration.bias)
    {
        esp_gpio_config.pull_up_en = GPIO_PULLUP_ENABLE;
        esp_gpio_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    else if(bsp::GPIOBIAS_PULLDOWN == configuration.bias)
    {
        esp_gpio_config.pull_up_en = GPIO_PULLUP_DISABLE;
        esp_gpio_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    gpio_config(&esp_gpio_config);
}

void EspGpioHandler::RegisterCallback(const bsp::GpioPin pin, std::function<void(const bsp::GpioPin pin)> callback)
{
    // Assign event handler
    event_handlers[pin] = callback;

    if (!isr_service_installed)
    {
        // Create a queue to handle GPIO event from ISR
        event_queue_handle = xQueueCreateStatic(GPIO_ESP_EVENT_QUEUE_SIZE, sizeof(bsp::GpioPin), event_queue_buffer, &event_queue);

        // Start task to handle events in the queue
        xTaskCreate(GpioEspEventHandlerTask, GPIO_ESP_TASK_NAME, GPIO_ESP_TASK_STACK_DEPTH, NULL, GPIO_ESP_TASK_PRIORITY, NULL);

        // Install GPIO ISR service
        gpio_install_isr_service(GPIO_ESP_INTR_FLAG_DEFAULT);
        isr_service_installed = true;
    }

    // Hook ISR handler for specific GPIO pins
    gpio_isr_handler_add((gpio_num_t)pin, GpioEspIsrHandler, (void *)pin);
}

void EspGpioHandler::SetLevel(const bsp::GpioPin pin, const bsp::GpioLevel level)
{
    gpio_set_level((gpio_num_t)pin, level);
}

bsp::GpioLevel EspGpioHandler::GetLevel(const bsp::GpioPin pin)
{
    return (bsp::GpioLevel)gpio_get_level((gpio_num_t)pin);
}

} // namespace bsp_esp

static void GpioEspEventHandlerTask(void *arg)
{
    bsp::GpioPin pin;

    for (;;)
    {
        if (xQueueReceive(event_queue_handle, &pin, portMAX_DELAY))
        {
            std::unordered_map<bsp::GpioPin, std::function<void(const bsp::GpioPin pin)>>::iterator event_handler = event_handlers.find(pin);

            if (event_handlers.end() != event_handler)
            {
                event_handler->second(pin);
            }
        }
    }

    vTaskDelete(NULL);
}

static void IRAM_ATTR GpioEspIsrHandler(void *arg)
{
    bsp::GpioPin pin = (bsp::GpioPin)arg;

    xQueueSendFromISR(event_queue_handle, &pin, NULL);
}

static gpio_mode_t ConvertGpioMode(const bsp::GpioMode mode)
{
    gpio_mode_t gpio_mode = GPIO_MODE_DISABLE;

    switch (mode)
    {
    case bsp::GPIOMODE_INPUT:
        gpio_mode = GPIO_MODE_INPUT;
        break;
    case bsp::GPIOMODE_OUTPUT:
        gpio_mode = GPIO_MODE_OUTPUT;
        break;
    default:
        break;
    }

    return gpio_mode;
}

static gpio_int_type_t ConvertGpioInterrupt(const bsp::GpioInterrupt interrupt)
{
    gpio_int_type_t gpio_interrupt = GPIO_INTR_DISABLE;

    switch (interrupt)
    {
    case bsp::GPIOINTERRUPT_DISABLED:
        break;
    case bsp::GPIOINTERRUPT_RISING:
        gpio_interrupt = GPIO_INTR_POSEDGE;
        break;
    case bsp::GPIOINTERRUPT_FALLING:
        gpio_interrupt = GPIO_INTR_NEGEDGE;
        break;
    case bsp::GPIOINTERRUPT_CHANGE:
        gpio_interrupt = GPIO_INTR_ANYEDGE;
        break;
    default:
        break;
    }

    return gpio_interrupt;
}
