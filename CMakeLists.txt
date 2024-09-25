cmake_minimum_required(VERSION 3.30)
project(trains LANGUAGES CXX)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/src/common)
set(MODULES_DIR ${CMAKE_SOURCE_DIR}/src/modules)
set(CTC_DIR ${MODULES_DIR}/ctc)
set(WAYSIDE_CONTROLLER_DIR ${MODULES_DIR}/wayside_controller)
set(TRACK_MODEL_DIR ${MODULES_DIR}/track_model)
set(TRAIN_MODEL_DIR ${MODULES_DIR}/train_model)
set(TRAIN_CONTROLLER_DIR ${MODULES_DIR}/train_controller)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

set(COMMON_INC_DIR ${COMMON_DIR}/inc)
set(CTC_INC_DIR ${CTC_DIR}/inc)
set(WAYSIDE_CONTROLLER_INC_DIR ${WAYSIDE_CONTROLLER_DIR}/inc)
set(TRACK_MODEL_INC_DIR ${TRACK_MODEL_DIR}/inc)
set(TRAIN_MODEL_INC_DIR ${TRAIN_MODEL_DIR}/inc)
set(TRAIN_CONTROLLER_INC_DIR ${TRAIN_CONTROLLER_DIR}/inc)
set(INC_DIRS
  ${COMMON_INC_DIR}
  ${CTC_INC_DIR}
  ${WAYSIDE_CONTROLLER_INC_DIR}
  ${TRACK_MODEL_INC_DIR}
  ${TRAIN_MODEL_INC_DIR}
  ${TRAIN_CONTROLLER_INC_DIR}
)

file(GLOB CTC_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${CTC_DIR}/src/*.cc"
)
file(GLOB WAYSIDE_CONTROLLER_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${WAYSIDE_CONTROLLER_DIR}/src/*.cc"
)
file(GLOB TRACK_MODEL_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${TRACK_MODEL_DIR}/src/*.cc"
)
file(GLOB TRAIN_MODEL_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${TRAIN_MODEL_DIR}/src/*.cc"
)
file(GLOB TRAIN_CONTROLLER_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${TRAIN_CONTROLLER_DIR}/src/*.cc"
)

set(SLINT_STYLE cosmic-dark)

find_package(Slint REQUIRED)

################################################################################
# CTC test build
################################################################################
add_executable(ctc_tests
  ${TESTS_DIR}/ctc/ctc_tests.cc
  ${CTC_SRCS}
)
target_include_directories(ctc_tests PRIVATE ${INC_DIRS})

################################################################################
# Wayside Controller test build
################################################################################
add_executable(wayside_controller_tests
  ${TESTS_DIR}/wayside_controller/wayside_controller_tests.cc
  ${WAYSIDE_CONTROLLER_SRCS})
target_include_directories(wayside_controller_tests PRIVATE ${INC_DIRS})

################################################################################
# Track Model test build
################################################################################
add_executable(track_model_tests
  ${TESTS_DIR}/track_model/track_model_tests.cc
  ${TRACK_MODEL_SRCS}
)
target_include_directories(track_model_tests PRIVATE ${INC_DIRS})

################################################################################
# Train Model test build
################################################################################
add_executable(train_model_tests
  ${TESTS_DIR}/train_model/train_model_tests.cc
  ${TRAIN_MODEL_SRCS}
)
target_include_directories(train_model_tests PRIVATE ${INC_DIRS})

################################################################################
# Train Controller test build
################################################################################
add_executable(train_controller_tests
  ${TESTS_DIR}/train_controller/train_controller_tests.cc
  ${TRAIN_CONTROLLER_SRCS}
)
target_include_directories(train_controller_tests PRIVATE ${INC_DIRS})

################################################################################
# Trains build
################################################################################
if (WIN32)
    add_executable(trains
      WIN32
      src/main.cpp
      ${CTC_SRCS}
      ${WAYSIDE_CONTROLLER_SRCS}
      ${TRACK_MODEL_SRCS}
      ${TRAIN_MODEL_SRCS}
      ${TRAIN_CONTROLLER_SRCS}
    )
else()
    add_executable(trains
      src/main.cpp
      ${CTC_SRCS}
      ${WAYSIDE_CONTROLLER_SRCS}
      ${TRACK_MODEL_SRCS}
      ${TRAIN_MODEL_SRCS}
      ${TRAIN_CONTROLLER_SRCS}
    )
endif()
target_include_directories(trains PRIVATE ${INC_DIRS})
target_link_libraries(trains PRIVATE Slint::Slint)
slint_target_sources(trains ui/launcher.slint)
if (WIN32)
    add_custom_command(TARGET trains POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:trains> $<TARGET_FILE_DIR:trains> COMMAND_EXPAND_LISTS)
endif()
