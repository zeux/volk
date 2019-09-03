# Tests using volk through the package config.
cmake_minimum_required(VERSION 3.0)
project(volk-usage C)

find_package(volk CONFIG REQUIRED)

add_executable(volk-usage main.c)
target_link_libraries(volk-usage PRIVATE volk::volk)