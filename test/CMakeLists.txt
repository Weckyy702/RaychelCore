if(NOT CATCH_2_EXTERNAL)
    find_package(Catch2 CONFIG REQUIRED)
endif()
if(NOT RAYCHEL_LOGGER_EXTERNAL)
    find_package(RaychelLogger REQUIRED)
endif()


file(GLOB_RECURSE RAYCHELCORE_TEST_SOURCES "*.test.cpp")

add_executable(Unit_test
    ${RAYCHELCORE_TEST_SOURCES}
)

target_compile_features(Unit_test PUBLIC cxx_std_20)
if(${MSVC})
    target_compile_definitions(Unit_test PUBLIC
        /W4
        /wd4068 #unknown pragma
    )
else()
    target_compile_options(Unit_test PUBLIC
        -Wall
        -Wextra
        -Wconversion
        -Werror
        -Wpedantic
    )
endif()

target_link_libraries(Unit_test PUBLIC
    RaychelCore
    Catch2::Catch2
)


include(Catch)
catch_discover_tests(Unit_test)