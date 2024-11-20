set(sources "wifi.c")
set(includes "include")

idf_component_register(
    SRCS ${sources}
    INCLUDE_DIRS ${includes}
    REQUIRES wifi esp_wifi nvs_flash
)
