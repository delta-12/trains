#include <memory>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "controller_port.h"
#include "green_line_blocks.h"
#include "hardware_wayside_controller_handler.h"
#include "tcp_port.h"
#include "wifi.h"

static const char *const kLogTag = "HARDWARE WAYSIDE";

extern "C" void app_main(void)
{
    /* Startup message */
    ESP_LOGI(kLogTag, "Startup...");
    ESP_LOGI(kLogTag, "Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(kLogTag, "IDF version: %s", esp_get_idf_version());

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize Wifi */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    Wifi_Init();
    Wifi_Start();

    std::shared_ptr<EspTcpPort> tcp_port = std::make_shared<EspTcpPort>("10.0.0.172", 8080);
    wayside_controller::HardwareWaysideControllerHandler<1024> wayside_controller_handler(1,
                                                                                    types::TrackId::TRACKID_GREEN,
                                                                                    wayside_controller::kGreenLineBlocksWayside0,
                                                                                    controller_network::BuildBasicControllerPort<1024>(std::static_pointer_cast<types::Port>(tcp_port)));
    size_t i = 0;
    while (!tcp_port->Connected() && (i < 5))
    {
        tcp_port->Connect("10.0.0.172", 8080);
        i++;
    }

    if (tcp_port->Connected())
    {
        char data[] = "foobar";
        tcp_port->Send((uint8_t *)data, 7);

        wayside_controller_handler.Connect();
    }

    while (true)
    {
        size_t bytes = tcp_port->ReceiveAvailable();
        if (bytes > 0)
        {
            ESP_LOGI(kLogTag, "Bytes received: %d", bytes);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}