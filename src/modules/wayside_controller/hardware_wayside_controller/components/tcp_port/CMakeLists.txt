set(sources "tcp_port.cc")
set(includes "include")

set(TRAINS_ROOT_DIR "../../../../../../")
set(SRC_DIR ${TRAINS_ROOT_DIR}/src)
set(COMMON_DIR ${SRC_DIR}/common)
set(COMMON_INC_DIR ${COMMON_DIR}/inc)

idf_component_register(
    SRCS ${sources}
    INCLUDE_DIRS ${includes} ${COMMON_INC_DIR}
    REQUIRES esp_netif
)
