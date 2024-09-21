cmake_minimum_required(VERSION 3.30)
project(trains LANGUAGES CXX)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SLINT_STYLE cosmic-dark)

find_package(Slint REQUIRED)

if (WIN32)
    add_executable(trains WIN32 src/main.cpp)
else()
    add_executable(trains src/main.cpp)
endif()
target_link_libraries(trains PRIVATE Slint::Slint)
slint_target_sources(trains ui/launcher.slint)
if (WIN32)
    add_custom_command(TARGET trains POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:trains> $<TARGET_FILE_DIR:trains> COMMAND_EXPAND_LISTS)
endif()
