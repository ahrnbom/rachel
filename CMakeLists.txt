cmake_minimum_required(VERSION 3.10)
project(rachel_project VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rachel INTERFACE)

target_include_directories(rachel INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/nodes/include>
    $<INSTALL_INTERFACE:nodes/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/default_nodes/include>
    $<INSTALL_INTERFACE:default_nodes/include>
)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# Custom nodes go here

add_library(some_node nodes/src/some_node.cpp)
target_link_libraries(some_node rachel)

add_library(other_node nodes/src/other_node.cpp)
target_link_libraries(other_node rachel)

# End of custom nodes 

# Included default nodes go here

add_library(transforms_node default_nodes/src/transforms_node.cpp)
target_link_libraries(transforms_node rachel)

# End of default nodes

add_library(rachel_impl src/rachel.cpp)
target_link_libraries(rachel_impl rachel)

add_executable(rachel_example src/main.cpp)

# Make sure to include all custom and default nodes here
target_link_libraries(rachel_example PUBLIC rachel rachel_impl spdlog::spdlog nlohmann_json::nlohmann_json some_node other_node transforms_node) 
