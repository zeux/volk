# Imports an installed volk target.

cmake_minimum_required(VERSION 3.0)
set(example_name "cmake-usage-target-installed-example")

project(${example_name} LANGUAGES C)

# Include volk from a CMake package config.
# CMAKE_PREFIX_PATH or volk_DIR must be set properly.
find_package(volk CONFIG REQUIRED)

add_executable(${example_name} main.c)
target_link_libraries(${example_name} PRIVATE volk::volk)
