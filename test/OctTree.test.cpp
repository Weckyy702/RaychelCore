#include "RaychelCore/OctTree.h"

#include "catch2/catch.hpp"

#include <chrono>
#include <cstddef>
#include <memory_resource>
#include <random>

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

    for (std::size_t i{}; i != 100; ++i) {
        REQUIRE(tree.insert(vec3{dist(rng), dist(rng), dist(rng)}));
        REQUIRE(tree.size() == i + 1);
    }
}

TEST_CASE("OctTree: get closest points")
{
    Raychel::OcTree<vec3, 5> tree{vec3{0, 0, 0}, vec3{100, 100, 100}};

    {
        const auto maybe_closest_item = tree.closest_to(vec3{0, 0, 0});

        REQUIRE_FALSE(maybe_closest_item.has_value());
    }

    tree.insert(vec3{10, 10, 10});

    {
        const auto entry = tree.closest_to(vec3{0, 0, 0});

        REQUIRE(entry.has_value());
        const auto [closest_point, _] = entry.value();
        REQUIRE(closest_point == vec3{10, 10, 10});
    }

    tree.insert(vec3{100, 100, 100});
    tree.insert(vec3{75, 75, 75});

    {
        const auto item = tree.closest_to(vec3{0, 0, 0});

        REQUIRE(item.has_value());
        const auto [point, _] = item.value();
        REQUIRE(point == vec3{10, 10, 10});
    }

    {
        std::mt19937 rng{1234};
        std::uniform_real_distribution<> dist{0., 100.};

        vec3 closest_point{0, 0, 0};
        double closest_distance = 1e9;

        for (std::size_t i{}; i != 100'000; ++i) {
            vec3 p{dist(rng), dist(rng), dist(rng)};
            const auto distance = Raychel::details::GetDistanceToPoint{}(p, vec3{50, 50, 50});

            if (distance < closest_distance) {
                closest_point = p;
                closest_distance = distance;
            }

            tree.insert(p);
        }

        const auto start = std::chrono::high_resolution_clock::now();
        const auto maybe_item = tree.closest_to(vec3{50, 50, 50});

        REQUIRE(maybe_item.has_value());

        const auto [point, distance] = maybe_item.value();

        REQUIRE(point == closest_point);
        REQUIRE(distance == closest_distance);

        std::cerr << duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start) << '\n';
    }
}
