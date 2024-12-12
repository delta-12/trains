cmake_minimum_required(VERSION 3.30)
project(trains LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-requires -fdiagnostics-color=always") # suppress missing-requires warning to build protobufs

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj -static-libgcc -static-libstdc++ -static")
else()
    set(SRC_COMPILE_OPTIONS -Wall -Wextra -Werror)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_compile_definitions(
    $<$<CONFIG:Debug>:LOGGER>
)

set(TEST_COMPILE_OPTIONS ${SRC_COMPILE_OPTIONS} -g -O0 --coverage)
option(ABSL_PROPAGATE_CXX_STD "Propagate C++ standard to abseil-cpp" ON)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(ASIO_DIR ${EXTERNAL_DIR}/asio)
set(PROTOBUF_INSTALL_DIR ${EXTERNAL_DIR}/protobuf/_build/protobuf-install)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BSP_DIR ${SRC_DIR}/bsp)
set(COMMON_DIR ${SRC_DIR}/common)
set(CONTROLLER_NETWORK_DIR ${SRC_DIR}/controller_network)
set(CONTROLLER_NETWORK_MESSAGES_DIR ${CONTROLLER_NETWORK_DIR}/messages)
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

set(ASIO_INC_DIR ${ASIO_DIR}/asio/include)
set(COMMON_INC_DIR ${COMMON_DIR}/inc)
set(CONTROLLER_NETWORK_INC_DIR ${CONTROLLER_NETWORK_DIR}/inc)
set(CTC_INC_DIR ${CTC_DIR}/inc)
set(WAYSIDE_CONTROLLER_INC_DIR ${WAYSIDE_CONTROLLER_DIR}/inc)
set(TRACK_MODEL_INC_DIR ${TRACK_MODEL_DIR}/inc)
set(TRAIN_MODEL_INC_DIR ${TRAIN_MODEL_DIR}/inc)
set(TRAIN_CONTROLLER_INC_DIR ${TRAIN_CONTROLLER_DIR}/inc)
set(SIMULATOR_INC_DIR ${SIMULATOR_DIR}/inc)
set(PROTOBUF_INC_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INSTALL_DIR}/include
)
set(EXTERNAL_INC_DIRS
    ${ASIO_INC_DIR}
    ${PROTOBUF_INC_DIRS}
)
set(INTERNAL_INC_DIRS
    ${BSP_DIR}
    ${COMMON_INC_DIR}
    ${CONTROLLER_NETWORK_INC_DIR}
    ${CTC_INC_DIR}
    ${WAYSIDE_CONTROLLER_INC_DIR}
    ${TRACK_MODEL_INC_DIR}
    ${TRAIN_MODEL_INC_DIR}
    ${TRAIN_CONTROLLER_INC_DIR}
    ${SIMULATOR_INC_DIR}
)
set(INC_DIRS
    ${EXTERNAL_INC_DIRS}
    ${INTERNAL_INC_DIRS}
)

file(GLOB COMMON_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${COMMON_DIR}/src/*.cc"
)
file(GLOB CONTROLLER_NETWORK_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CONTROLLER_NETWORK_DIR}/src/*.cc"
)
file(GLOB CONTROLLER_NETWORK_MESSAGE_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CONTROLLER_NETWORK_DIR}/messages/*.proto"
)
file(GLOB CTC_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CTC_DIR}/src/*.cc"
)
file(GLOB CTC_HANDLER_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CTC_DIR}/src/*_handler.cc"
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
    ${CONTROLLER_NETWORK_SRCS}
    ${CTC_SRCS}
    ${WAYSIDE_CONTROLLER_SRCS}
    ${TRACK_MODEL_SRCS}
    ${TRAIN_MODEL_SRCS}
    ${TRAIN_CONTROLLER_SRCS}
    ${SIMULATOR_SRCS}
)

# Copy of TRAINS_SRCS exclude backend-frontend handlers to avoid launcher.h not found
set(TEST_TRAINS_SRCS ${TRAINS_SRCS})
list(REMOVE_ITEM TEST_TRAINS_SRCS ${CTC_HANDLER_SRCS})

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
# Controller Messages
################################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/external/protobuf/third_party/abseil-cpp)
find_package(utf8_range CONFIG REQUIRED HINTS ${PROTOBUF_INSTALL_DIR})
find_package(Protobuf CONFIG REQUIRED HINTS ${PROTOBUF_INSTALL_DIR})
add_library(controller_messages)
protobuf_generate(
	TARGET controller_messages
	LANGUAGE cpp
	PROTOS ${CONTROLLER_NETWORK_MESSAGE_SRCS}
	IMPORT_DIRS ${CONTROLLER_NETWORK_MESSAGES_DIR}
)
target_link_libraries(controller_messages PRIVATE protobuf::libprotobuf)
target_include_directories(controller_messages PRIVATE ${PROTOBUF_INC_DIRS})

################################################################################
# Trains build
################################################################################
if (WIN32)
    add_executable(trains WIN32 ${SRC_DIR}/main.cc ${TRAINS_SRCS})
    target_link_libraries(trains PRIVATE wsock32 ws2_32)
else()
    add_executable(trains ${SRC_DIR}/main.cc ${TRAINS_SRCS})
endif()
target_compile_options(trains PRIVATE ${SRC_COMPILE_OPTIONS})
target_include_directories(trains PRIVATE ${INC_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(trains PRIVATE Slint::Slint controller_messages)
slint_target_sources(trains ${UI_DIR}/launcher.slint NAMESPACE ui)
if (WIN32)
    add_custom_command(TARGET trains POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:trains> $<TARGET_FILE_DIR:trains> COMMAND_EXPAND_LISTS)
endif()

################################################################################
# Unit tests
################################################################################
enable_testing()
include(GoogleTest)
add_library(trains_lib STATIC ${TEST_TRAINS_SRCS}) # Add static library to avoid re-compiling sources for tests
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(trains_lib PRIVATE ${TEST_COMPILE_OPTIONS})
    target_link_options(trains_lib PRIVATE --coverage)
endif()
target_include_directories(trains_lib PRIVATE ${INC_DIRS})
target_link_libraries(trains_lib PRIVATE controller_messages)
foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TEST_NAME} PRIVATE ${TEST_COMPILE_OPTIONS})
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    endif()
    target_include_directories(${TEST_NAME} PRIVATE ${INC_DIRS})
    target_link_libraries(${TEST_NAME} PRIVATE trains_lib GTest::gtest_main GTest::gmock)
    gtest_discover_tests(${TEST_NAME})
endforeach()

################################################################################
# Coverage
################################################################################
find_program(GCOVR_PATH gcovr
    HINTS ~/.local/bin/
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(GCOVR_PATH)
        message(STATUS "Found Gcovr at: ${GCOVR_PATH}")
        set(GCOVR_ARGS -r ${CMAKE_BINARY_DIR} -f ${SRC_DIR} --exclude-throw-branches)
        if(DEFINED ENV{GCOV_EXECUTABLE})
            set(GCOVR_ARGS --gcov-executable $ENV{GCOV_EXECUTABLE} ${GCOVR_ARGS})
        endif()
        add_custom_target(
            coverage
            COMMAND ${GCOVR_PATH} ${GCOVR_ARGS} --html-details --html-theme github.dark-green -o coverage.html
            COMMENT "Generating code coverage report..."
            DEPENDS test
        )
        add_custom_target(
            coverage-no-test
            COMMAND ${GCOVR_PATH} ${GCOVR_ARGS} --html-details --html-theme github.dark-green -o coverage.html
            COMMENT "Generating code coverage report..."
        )
        add_custom_target(
            sonarqube-coverage
            COMMAND ${GCOVR_PATH} ${GCOVR_ARGS} --sonarqube -o coverage.xml
            COMMENT "Generating code coverage report..."
        )
    else()
        message(WARNING "Gcovr not found.")
    endif()
else()
    message(WARNING "Gcovr does not support compiler.")
endif()