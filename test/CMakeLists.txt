find_package(Catch2 CONFIG REQUIRED)
find_package(RaychelLogger REQUIRED)


file(GLOB_RECURSE RAYCHELCORE_TEST_SOURCES "*.test.cpp")

add_executable(Unit_test
    ${RAYCHELCORE_TEST_SOURCES}
)

target_compile_features(Unit_test PUBLIC cxx_std_20)
target_compile_options(Unit_test PUBLIC
    -Wall
    -Wextra
    -Wconversion
    -Werror
    -Wpedantic
)

target_link_libraries(Unit_test
    INTERFACE RaychelCore
    PUBLIC RaychelLogger
    PUBLIC Catch2::Catch2
)


include(Catch)
catch_discover_tests(Unit_test)









#TODO: remove this and incorporate it into the regular tests
add_executable(ArgumentParserTest
    ArgumentParser.cpp
)

target_compile_features(ArgumentParserTest PUBLIC cxx_std_20)
target_compile_options(ArgumentParserTest PUBLIC
    -Wall
    -Wextra
    -Wconversion
    -Werror
    -Wpedantic
)

target_link_libraries(ArgumentParserTest
    PUBLIC RaychelCore
    PUBLIC RaychelLogger
    PUBLIC Catch2::Catch2
)