set(TRAINS_ROOT_DIR "../../../../../")
set(SRC_DIR ${TRAINS_ROOT_DIR}/src)
set(BSP_DIR ${SRC_DIR}/bsp)
set(COMMON_DIR ${SRC_DIR}/common)
set(CONTROLLER_NETWORK_DIR ${SRC_DIR}/controller_network)
set(MODULES_DIR ${SRC_DIR}/modules)
set(WAYSIDE_CONTROLLER_DIR ${MODULES_DIR}/wayside_controller)

set(COMMON_INC_DIR ${COMMON_DIR}/inc)
set(CONTROLLER_NETWORK_INC_DIR ${CONTROLLER_NETWORK_DIR}/inc)
set(WAYSIDE_CONTROLLER_INC_DIR ${WAYSIDE_CONTROLLER_DIR}/inc)
set(INC_DIRS
    ${BSP_DIR}
    ${COMMON_INC_DIR}
    ${CONTROLLER_NETWORK_INC_DIR}
    ${WAYSIDE_CONTROLLER_INC_DIR}
)

idf_component_register(SRCS "hardware_wayside_controller.cc" "display.c"
                       INCLUDE_DIRS "." ${INC_DIRS})
