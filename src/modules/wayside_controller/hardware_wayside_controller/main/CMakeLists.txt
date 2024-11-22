set(TRAINS_ROOT_DIR "../../../../../")
set(SRC_DIR ${TRAINS_ROOT_DIR}/src)
set(BSP_DIR ${SRC_DIR}/bsp)
set(COMMON_DIR ${SRC_DIR}/common)
set(CONTROLLER_NETWORK_DIR ${SRC_DIR}/controller_network)
set(MODULES_DIR ${SRC_DIR}/modules)
set(WAYSIDE_CONTROLLER_DIR ${MODULES_DIR}/wayside_controller)
set(CONTROLLER_MESSAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../controller_messages)

set(COMMON_INC_DIR ${COMMON_DIR}/inc)
set(CONTROLLER_NETWORK_INC_DIR ${CONTROLLER_NETWORK_DIR}/inc)
set(WAYSIDE_CONTROLLER_INC_DIR ${WAYSIDE_CONTROLLER_DIR}/inc)
set(INC_DIRS
    ${BSP_DIR}
    ${COMMON_INC_DIR}
    ${CONTROLLER_NETWORK_INC_DIR}
    ${WAYSIDE_CONTROLLER_INC_DIR}
    ${CONTROLLER_MESSAGE_DIR}
)

set(CONTROLLER_MESSAGE_SRCS
    ${CONTROLLER_MESSAGE_DIR}/pb_common.c
    ${CONTROLLER_MESSAGE_DIR}/pb_decode.c
    ${CONTROLLER_MESSAGE_DIR}/pb_encode.c
    ${CONTROLLER_MESSAGE_DIR}/block_occupancies.pb.c
    ${CONTROLLER_MESSAGE_DIR}/block_states.pb.c
    ${CONTROLLER_MESSAGE_DIR}/connection.pb.c
    ${CONTROLLER_MESSAGE_DIR}/common.pb.c
    ${CONTROLLER_MESSAGE_DIR}/track_circuit_data.pb.c
)

idf_component_register(SRCS "hardware_wayside_controller.cc" "display.c" ${COMMON_DIR}/src/convert.cc ${CONTROLLER_MESSAGE_SRCS} ${WAYSIDE_CONTROLLER_DIR}/src/wayside_controller.cc
                       INCLUDE_DIRS "." ${INC_DIRS})
