#include "RaychelCore/OctTree.h"

#include "catch2/catch.hpp"

//Non-default constructible vec3
struct vec3
{
    vec3() = delete;

    vec3(double _x, double _y, double _z) : x{_x}, y{_y}, z{_z}
    {}

    double x, y, z;
};

using OctTree = Raychel::OctTree<vec3>;

TEST_CASE("OctTree: creating OctTrees")
{
    OctTree tree{{vec3{-100, -100, -100}, vec3{100, 100, 100}}};

    REQUIRE(tree.size() == 0);
}

TEST_CASE("OctTree: Inserting N elements into the tree")
{
    OctTree tree{{vec3{-100, -100, -100}, vec3{100, 100, 100}}};

    for (std::size_t _i{}; _i != 5; ++_i) {
        const auto i = static_cast<double>(_i) * 5.0;

        REQUIRE(tree.insert(vec3{i, i, i}));
        REQUIRE(tree.size() == _i + 1);
    }
}

TEST_CASE("OctTree: Subdivison when inserting more than N elements")
{
    OctTree tree{{vec3{-100, -100, -100}, vec3{100, 100, 100}}};

    std::mt19937 rng{12345};
    std::uniform_real_distribution<double> dist{-100.0, 100.0};

    for (std::size_t _i{}; _i != 100; ++_i) {
        REQUIRE(tree.insert(vec3{dist(rng), dist(rng), dist(rng)}));
        REQUIRE(tree.size() == _i + 1);
    }
}

TEST_CASE("OctTree: get closest points")
{
    OctTree tree{{vec3{-100, -100, -100}, vec3{100, 100, 100}}};
}
