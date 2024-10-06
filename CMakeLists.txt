cmake_minimum_required(VERSION 3.30)
project(trains LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(COMMON_DIR ${SRC_DIR}/common)
set(MODULES_DIR ${SRC_DIR}/modules)
set(CTC_DIR ${MODULES_DIR}/ctc)
set(WAYSIDE_CONTROLLER_DIR ${MODULES_DIR}/wayside_controller)
set(TRACK_MODEL_DIR ${MODULES_DIR}/track_model)
set(TRAIN_MODEL_DIR ${MODULES_DIR}/train_model)
set(TRAIN_CONTROLLER_DIR ${MODULES_DIR}/train_controller)
set(SIMULATOR_DIR ${SRC_DIR}/simulator)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)
set(UI_DIR ${CMAKE_SOURCE_DIR}/ui)
set(TOOLS_DIR ${CMAKE_SOURCE_DIR}/tools)

set(COMMON_INC_DIR ${COMMON_DIR}/inc)
set(CTC_INC_DIR ${CTC_DIR}/inc)
set(WAYSIDE_CONTROLLER_INC_DIR ${WAYSIDE_CONTROLLER_DIR}/inc)
set(TRACK_MODEL_INC_DIR ${TRACK_MODEL_DIR}/inc)
set(TRAIN_MODEL_INC_DIR ${TRAIN_MODEL_DIR}/inc)
set(TRAIN_CONTROLLER_INC_DIR ${TRAIN_CONTROLLER_DIR}/inc)
set(SIMULATOR_INC_DIR ${SIMULATOR_DIR}/inc)
set(INC_DIRS
  ${COMMON_INC_DIR}
  ${CTC_INC_DIR}
  ${WAYSIDE_CONTROLLER_INC_DIR}
  ${TRACK_MODEL_INC_DIR}
  ${TRAIN_MODEL_INC_DIR}
  ${TRAIN_CONTROLLER_INC_DIR}
  ${SIMULATOR_INC_DIR}
)

file(GLOB COMMON_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${COMMON_DIR}/src/*.cc"
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
file(GLOB SIMULATOR_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${SIMULATOR_DIR}/src/*.cc"
)
file(GLOB_RECURSE TEST_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
  "${TESTS_DIR}/*_tests.cc"
)

set(TRAINS_SRCS
  ${COMMON_SRCS}
  ${CTC_SRCS}
  ${WAYSIDE_CONTROLLER_SRCS}
  ${TRACK_MODEL_SRCS}
  ${TRAIN_MODEL_SRCS}
  ${TRAIN_CONTROLLER_SRCS}
  ${SIMULATOR_SRCS}
)

set(SLINT_STYLE cosmic-dark)

find_package(Slint REQUIRED)

include(${TOOLS_DIR}/uncrustify/uncrustify.cmake)
include(${TOOLS_DIR}/cppcheck/cppcheck.cmake)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/a1e255a582377e1006bb88a408ac3f933ba7c916.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

################################################################################
# CTC UI test build
################################################################################
add_executable(ctc_tests_ui
  ${TESTS_DIR}/modules/ctc/ctc_tests_ui.cc
  ${CTC_SRCS}
  ${COMMON_SRCS}
)
target_include_directories(ctc_tests_ui PRIVATE ${INC_DIRS})
target_link_libraries(ctc_tests_ui PRIVATE Slint::Slint)
slint_target_sources(ctc_tests_ui ${UI_DIR}/ctc_ui.slint NAMESPACE ui)

################################################################################
# Wayside Controller UI test build
################################################################################
add_executable(wayside_controller_tests_ui
  ${TESTS_DIR}/modules/wayside_controller_tests_ui.cc
  ${WAYSIDE_CONTROLLER_SRCS}
  ${COMMON_SRCS}
)
target_include_directories(wayside_controller_tests_ui PRIVATE ${INC_DIRS})
target_link_libraries(wayside_controller_tests_ui PRIVATE Slint::Slint)
slint_target_sources(wayside_controller_tests_ui ${UI_DIR}/wayside_controller_ui.slint NAMESPACE ui)

################################################################################
# Track Model UI test build
################################################################################
add_executable(track_model_tests_ui
  ${TESTS_DIR}/modules/track_model_tests_ui.cc
  ${TRACK_MODEL_SRCS}
)
target_include_directories(track_model_tests_ui PRIVATE ${INC_DIRS})
target_link_libraries(track_model_tests_ui PRIVATE Slint::Slint)
slint_target_sources(track_model_tests_ui ${UI_DIR}/track_model_ui.slint NAMESPACE ui)

################################################################################
# Train Model UI test build
################################################################################
add_executable(train_model_tests_ui
  ${TESTS_DIR}/modules/train_model_tests_ui.cc
  ${TRAIN_MODEL_SRCS}
)
target_include_directories(train_model_tests_ui PRIVATE ${INC_DIRS})
target_link_libraries(train_model_tests_ui PRIVATE Slint::Slint)
slint_target_sources(train_model_tests_ui ${UI_DIR}/train_model_ui.slint NAMESPACE ui)

################################################################################
# Train Controller UI test build
################################################################################
add_executable(train_controller_tests_ui
  ${TESTS_DIR}/modules/train_controller_tests_ui.cc
  ${TRAIN_CONTROLLER_SRCS}
)
target_include_directories(train_controller_tests_ui PRIVATE ${INC_DIRS})
target_link_libraries(train_controller_tests_ui PRIVATE Slint::Slint)
slint_target_sources(train_controller_tests_ui ${UI_DIR}/train_controller_ui.slint NAMESPACE ui)

################################################################################
# Trains build
################################################################################
if (WIN32)
    add_executable(trains WIN32 ${SRC_DIR}/main.cpp ${TRAINS_SRCS})
else()
    add_executable(trains ${SRC_DIR}/main.cpp ${TRAINS_SRCS})
endif()
target_include_directories(trains PRIVATE ${INC_DIRS})
target_link_libraries(trains PRIVATE Slint::Slint)
slint_target_sources(trains ${UI_DIR}/launcher.slint NAMESPACE ui)
if (WIN32)
    add_custom_command(TARGET trains POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:trains> $<TARGET_FILE_DIR:trains> COMMAND_EXPAND_LISTS)
endif()

################################################################################
# Unit tests
################################################################################
enable_testing()
include(GoogleTest)
foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    message(STATUS ${TEST_NAME} ${TEST_SRC})
    add_executable(${TEST_NAME} ${TEST_SRC} ${TRAINS_SRCS})
    target_include_directories(${TEST_NAME} PRIVATE ${INC_DIRS})
    target_link_libraries(${TEST_NAME} GTest::gtest_main)
    gtest_discover_tests(${TEST_NAME})
endforeach()