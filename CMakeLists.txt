cmake_minimum_required(VERSION 3.14)

option(RAYCHELCORE_BUILD_TESTS "Build the unit test executable" OFF)

project(
  RaychelCore
  VERSION 0.0.1
  LANGUAGES CXX)
# DEPENDENCIES
include(cmake/find_dependencies.cmake)

# HEADER FILES
set(RAYCHELCORE_HEADER_PATH "${RaychelCore_SOURCE_DIR}/include")
file(GLOB_RECURSE RAYCHELCORE_HEADERS "${RAYCHELCORE_HEADER_PATH}/*.h")

# INSTALLATION DIRECTORIES
include(GNUInstallDirs)
set(RAYCHELCORE_INSTALLED_INCLUDE_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(RAYCHELCORE_INSTALLED_HEADER_DIR
    ${RAYCHELCORE_INSTALLED_INCLUDE_DIR}/RaychelCore)
set(RAYCHELCORE_INSTALLED_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/RaychelCore)

# LIBRARY
add_library(RaychelCore INTERFACE)
target_include_directories(
  RaychelCore
  INTERFACE
    $<BUILD_INTERFACE:${RAYCHELCORE_HEADER_PATH}> # for headers when building
    $<INSTALL_INTERFACE:${RAYCHELCORE_INSTALLED_INCLUDE_DIR}> # for client in
                                                              # install mode
)

message(STATUS ${RAYCHELCORE_INSTALLED_HEADER_DIR})

if(NOT RAYCHEL_LOGGER_EXTERNAL)
  find_package(RaychelLogger REQUIRED)
endif()
target_link_libraries(RaychelCore INTERFACE RaychelLogger)

# INSTALLATION
install(TARGETS RaychelCore EXPORT RaychelCore)
install(FILES ${RAYCHELCORE_HEADERS}
        DESTINATION ${RAYCHELCORE_INSTALLED_HEADER_DIR})
install(FILES RaychelCoreConfig.cmake
        DESTINATION ${RAYCHELCORE_INSTALLED_LIB_DIR})
install(EXPORT RaychelCore DESTINATION ${RAYCHELCORE_INSTALLED_LIB_DIR})

# TESTS
if(RAYCHELCORE_BUILD_TESTS)
  include(CTest)
  add_subdirectory(test)
endif()
