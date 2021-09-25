include(FetchContent)

find_package(Catch2 QUIET)

if(NOT Catch2_FOUND)

    message(STATUS "Could not find a local installation of Catch2, downloading one off github...")

    FetchContent_Declare(CATCH_2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v2.13.6"
    )

    FetchContent_MakeAvailable(CATCH_2)

    set(CATCH_2_EXTERNAL true)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Catch2_SOURCE_DIR}/contrib)

endif()