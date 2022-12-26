#include "RaychelCore/SmallBuffer.h"

#include <catch2/catch.hpp>
#include <cstddef>
#include <iostream>

using Buffer = Raychel::SmallBuffer<16>;

struct SmallStruct
{
    std::size_t value{};

    ~SmallStruct() noexcept
    {
        std::cout << "SmallStruct destroyed!";
    }
};

struct BigStruct
{
    std::size_t data[16]{};

    ~BigStruct() noexcept
    {
        std::cout << "BigStruct destroyed!";
    }
};

TEST_CASE("SmallBuffer")
{
    Buffer buffer{SmallStruct{.value = 12}};
    REQUIRE(buffer.unsafe_get_value<SmallStruct>().value == 12U);
    REQUIRE_FALSE(buffer.is_heap());

    buffer.emplace(BigStruct{});

    REQUIRE(buffer.is_heap());
    REQUIRE(buffer.unsafe_get_value<BigStruct>().data[0] == 0U);
}
