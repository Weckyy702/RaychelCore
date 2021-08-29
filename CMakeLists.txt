cmake_minimum_required(VERSION 3.0)

project(RaychelCore VERSION 0.0.1 LANGUAGES CXX)



#HEADER FILES
set(RAYCHELCORE_HEADER_PATH "${RaychelCore_SOURCE_DIR}/include")
file(GLOB_RECURSE RAYCHELCORE_HEADERS "${RAYCHELCORE_HEADER_PATH}/*.h")

#INSTALLATION DIRECTORIES
include(GNUInstallDirs)
set(RAYCHELCORE_INSTALLED_HEADER_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}/RaychelCore)

#LIBRARY
add_library(RaychelCore INTERFACE
    ${RAYCHELCORE_HEADERS}
)
target_include_directories(RaychelCore INTERFACE
    $<BUILD_INTERFACE:${RAYCHELCORE_HEADER_PATH}> # for headers when building
    $<INSTALL_INTERFACE:${RAYCHELCORE_INSTALLED_HEADER_DIR}> # for client in install mode
)

find_package(RaychelLogger REQUIRED)
target_link_libraries(RaychelCore INTERFACE
    RaychelLogger
)

install(TARGETS RaychelCore
    EXPORT RaychelCore
)
install(FILES ${RAYCHELCORE_HEADERS}
    DESTINATION ${RAYCHELCORE_INSTALLED_HEADER_DIR}
)
install(FILES RaychelCoreConfig.cmake
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/RaychelCore
)