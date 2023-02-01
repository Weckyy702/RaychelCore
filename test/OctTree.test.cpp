#include "RaychelCore/OctTree.h"

#include "catch2/catch.hpp"

#include <cstddef>
#include <memory_resource>

//Non-default constructible vec3
struct vec3
{
    vec3() = delete;

    vec3(double _x, double _y, double _z) : x{_x}, y{_y}, z{_z}
    {}

    constexpr auto operator<=>(const vec3&) const noexcept = default;

    double x, y, z;
};

using OctTree = Raychel::OcTree<vec3, 10>;

TEST_CASE("OctTree: creating OctTrees")
{
    OctTree tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    REQUIRE(tree.size() == 0);
}

TEST_CASE("OctTree: Inserting N elements into the tree")
{
    OctTree tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    for (std::size_t _i{}; _i != 5; ++_i) {
        const auto i = static_cast<double>(_i) * 5.0;

        REQUIRE(tree.insert(vec3{i, i, i}));
        REQUIRE(tree.size() == _i + 1);
    }
}

TEST_CASE("OctTree: Subdivison when inserting more than N elements")
{
    OctTree tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    std::mt19937 rng{12345};
    std::uniform_real_distribution<double> dist{-100.0, 100.0};

    for (std::size_t _i{}; _i != 100; ++_i) {
        REQUIRE(tree.insert(vec3{dist(rng), dist(rng), dist(rng)}));
        REQUIRE(tree.size() == _i + 1);
    }
}

TEST_CASE("OctTree: get closest points")
{
    Raychel::OcTree<vec3, 1> tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    {
        const auto points = tree.closest_to(vec3{0, 0, 0});

        REQUIRE(points.empty());
    }

    tree.insert(vec3{10, 10, 10});

    {
        const auto points = tree.closest_to(vec3{0, 0, 0});

        REQUIRE(points.size() == 1);
        REQUIRE(points[0] == vec3{10, 10, 10});
    }

    {
        std::array<std::byte, 1024> buf{};
        std::pmr::monotonic_buffer_resource resource{buf.data(), buf.size(), std::pmr::null_memory_resource()};

        const auto points = tree.closest_to(vec3{0, 0, 0});

        REQUIRE(points.size() == 1);
        REQUIRE(points[0] == vec3{10, 10, 10});
    }

    tree.insert(vec3{75, 75, 75});
    tree.insert(vec3{50, 50, 50});

    {
        const auto points = tree.closest_to(vec3{0, 0, 0});

        tree.debug_print();

        REQUIRE(points.size() == 1);
        REQUIRE(points[0] == vec3{10, 10, 10});
    }
}
