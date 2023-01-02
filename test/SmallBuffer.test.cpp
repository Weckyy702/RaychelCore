#include "RaychelCore/SmallBuffer.h"

#include <catch2/catch.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>

using Buffer = Raychel::SmallBuffer<16>;

struct SmallStruct
{
    std::uint32_t value{};
};

struct BigStruct
{
    std::uint32_t data[16]{};
};

TEST_CASE("SmallBuffer: construction")
{
    Buffer buffer{SmallStruct{.value = 12}};
    REQUIRE(buffer.unsafe_get_value<SmallStruct>().value == 12);
    REQUIRE_FALSE(buffer.is_heap());

    buffer.emplace(BigStruct{});

    REQUIRE(buffer.is_heap());
    REQUIRE(buffer.unsafe_get_value<BigStruct>().data[0] == 0);
}

TEST_CASE("SmallBuffer: copy")
{
    Buffer buffer{SmallStruct{.value = 13}};
    REQUIRE(buffer.unsafe_get_value<SmallStruct>().value == 13);
    REQUIRE_FALSE(buffer.is_heap());

    Buffer copy{buffer};
    REQUIRE(copy.unsafe_get_value<SmallStruct>().value == 13);
    REQUIRE_FALSE(copy.is_heap());

    const auto temp = Buffer{BigStruct{}};

    copy = temp;

    REQUIRE(copy.unsafe_get_value<BigStruct>().data[0] == 0);
    REQUIRE(copy.is_heap());

    REQUIRE(temp.unsafe_get_value<BigStruct>().data[0] == 0);
    REQUIRE(temp.is_heap());
}

TEST_CASE("SmallBuffer: move")
{
    Buffer buffer{SmallStruct{.value = 14}};
    REQUIRE(buffer.unsafe_get_value<SmallStruct>().value == 14);
    REQUIRE_FALSE(buffer.is_heap());

    Buffer move{std::move(buffer)};
    REQUIRE(move.unsafe_get_value<SmallStruct>().value == 14);
    REQUIRE_FALSE(move.is_heap());

    REQUIRE(buffer.unsafe_get_ptr() == nullptr);

    move = Buffer{BigStruct{}};

    REQUIRE(move.unsafe_get_value<BigStruct>().data[0] == 0);
    REQUIRE(move.is_heap());
}
