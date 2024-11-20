#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

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

    EspTcpPort tcp_port("10.0.0.172", 8080);
    while(!tcp_port.Connected())
    {
        tcp_port.Connect("10.0.0.172", 8080);
    }
}