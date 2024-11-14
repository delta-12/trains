set(BSP_ESP_SRCS "src/gpio_esp.cc" "src/uart_esp.cc")
set(BSP_ESP_INC "inc" "../../../../../bsp")

idf_component_register(
    SRCS ${BSP_ESP_SRCS}
    INCLUDE_DIRS ${BSP_ESP_INC}
    PRIV_REQUIRES bsp_esp driver
)