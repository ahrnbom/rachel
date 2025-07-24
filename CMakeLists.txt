cmake_minimum_required(VERSION 3.10)
project(rachel_project VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a library from the header
add_library(rachel INTERFACE)

# Set include directories
target_include_directories(rachel INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_library(rachel_impl rachel.cpp)
target_link_libraries(rachel_impl PUBLIC rachel)

add_executable(rachel_example main.cpp)
target_link_libraries(rachel_example rachel)

