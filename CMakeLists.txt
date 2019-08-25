cmake_minimum_required(VERSION 3.0)

# Set the project version to the vulkan header version.
file(STRINGS volk.h matched_line REGEX "#define[ \t]+VOLK_HEADER_VERSION" LIMIT_INPUT 1000)
string(REGEX MATCH "([0-9\\.]+)$" parsed_version ${matched_line})
if(NOT parsed_version)
  message(FATAL_ERROR "Failed to parse VOLK_HEADER_VERSION!")
endif()

project(volk VERSION ${parsed_version} LANGUAGES C)

add_library(volk STATIC volk.c volk.h)
target_include_directories(volk PUBLIC 
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(MSVC)
  target_compile_options(volk PRIVATE /W4 /WX)
else()
  target_compile_options(volk PRIVATE -Wall -Wextra -Werror)
  target_link_libraries(volk PRIVATE dl)
endif()

find_package(Vulkan)
if(TARGET Vulkan::Vulkan) 
  target_link_libraries(volk PUBLIC Vulkan::Vulkan)
elseif(DEFINED ENV{VULKAN_SDK})
  if(IS_ABSOLUTE "$ENV{VULKAN_SDK}")
    target_include_directories(volk PUBLIC "$ENV{VULKAN_SDK}/include")
  else()
    message("Detected relative path to Vulkan SDK -- assuming CI build, skipping exporting Vulkan header include path.")
  endif()
endif()

# Installation

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/volk)

# Install header
install(FILES volk.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install library target and add it and any dependencies to export set.
install(
    TARGETS volk
    EXPORT volk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Actually write exported config w/ imported targets
install(EXPORT volk-targets
    FILE volkTargets.cmake
    NAMESPACE volk::
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file:
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion
)

# Configure config file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/volkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the fully generated config and configVersion files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)
