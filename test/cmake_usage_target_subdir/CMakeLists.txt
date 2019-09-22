# Include the volk target through add_subdirectory.

cmake_minimum_required(VERSION 3.0)
set(example_name "cmake-usage-target-subdir-example")

project(${example_name} LANGUAGES C)

# Include volk as part of the build tree to make the target known.
# The two-argument version of add_subdirectory allows adding non-subdirs.
add_subdirectory(../.. volk)

add_executable(${example_name} main.c)
target_link_libraries(${example_name} PRIVATE volk)
