cmake_minimum_required(VERSION 3.10)
project(rachel_project VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rachel INTERFACE)

target_include_directories(rachel INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(rachel_impl src/rachel.cpp)
target_link_libraries(rachel_impl PUBLIC rachel)

add_executable(rachel_example src/main.cpp)
target_link_libraries(rachel_example rachel_impl)

