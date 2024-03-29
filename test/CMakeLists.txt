if(NOT CATCH_2_EXTERNAL)
  find_package(Catch2 CONFIG REQUIRED)
endif()
if(NOT RAYCHEL_LOGGER_EXTERNAL)
  find_package(RaychelLogger REQUIRED)
endif()

file(GLOB_RECURSE RAYCHELCORE_TEST_SOURCES "*.test.cpp")

add_executable(RaychelCore_test ${RAYCHELCORE_TEST_SOURCES})

target_compile_features(RaychelCore_test PUBLIC cxx_std_20)
if(${MSVC})
  target_compile_definitions(
    RaychelCore_test PUBLIC /W4 /wd4068 # unknown pragma
  )
else()
  target_compile_options(
    RaychelCore_test
    PUBLIC -Wall -Wextra -Wconversion -Werror -Wpedantic
    PRIVATE -fsanitize=address,undefined)

  target_link_options(RaychelCore_test PRIVATE -fsanitize=address,undefined)

endif()

target_link_libraries(RaychelCore_test PUBLIC RaychelCore Catch2::Catch2)

include(Catch)
catch_discover_tests(RaychelCore_test)
