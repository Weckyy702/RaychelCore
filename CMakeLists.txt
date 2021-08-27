cmake_minimum_required(VERSION 3.0)

project(RaychelCore VERSION 0.0.1 LANGUAGES CXX)



#LIBRARY
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE 
    "include/"
)



find_package(RaychelLogger REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE
    RaychelLogger
)



#INSTALLATION
file(GLOB RAYCHEL_CORE_HEADERS RELATIVE "${PROJECT_SOURCE_DIR}" "include/*.h" )
include(GNUInstallDirs)
install(FILES 
    ${RAYCHEL_CORE_HEADERS}
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/RaychelCore"
)
install(FILES
    RaychelCoreConfig.cmake
    DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/RaychelCore"
)