cmake_minimum_required(VERSION 3.0)
cmake_policy(PUSH)
cmake_policy(SET CMP0048 NEW) # project(... VERSION ...) support

# Set the project version to the vulkan header version.
file(STRINGS volk.h matched_line REGEX "#define[ \t]+VOLK_HEADER_VERSION" LIMIT_INPUT 1000)
string(REGEX MATCH "([0-9\\.]+)$" parsed_version ${matched_line})
if(NOT parsed_version)
  message(FATAL_ERROR "Failed to parse VOLK_HEADER_VERSION!")
endif()

project(volk VERSION ${parsed_version} LANGUAGES NONE)

add_library(volk INTERFACE)
target_include_directories(volk INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:include>
)
if (NOT WIN32)
    target_link_libraries(volk INTERFACE dl)
endif()

# If CMake has the FindVulkan module and it works, use it.
# Otherwise silently rely on the environment variable.
find_package(Vulkan QUIET)

if(NOT VOLK_INSTALL)
  if(TARGET Vulkan::Vulkan)
    target_link_libraries(volk INTERFACE Vulkan::Vulkan)
  elseif(DEFINED ENV{VULKAN_SDK})
    target_include_directories(volk INTERFACE "$ENV{VULKAN_SDK}/include")
  endif()
endif()

option(VOLK_INSTALL "Create installation target" OFF)

if(VOLK_INSTALL)

  include(GNUInstallDirs)
  set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/volk)

  # Install files
  install(FILES volk.h volk.c DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # Install library target and add it and any dependencies to export set.
  install(TARGETS volk EXPORT volk-targets)

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
  configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/volkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
  )

  # Install the fully generated config and configVersion files
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
  )

endif()
cmake_policy(POP)
