#include "RaychelCore/OctTree.h"

#include "catch2/catch.hpp"

struct vec3
{
    double x, y, z;
};

using OctTree = Raychel::OctTree<vec3, 5, vec3>;

TEST_CASE("OctTree: creating OctTrees")
{
    OctTree tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    REQUIRE(tree.size() == 0);
}

TEST_CASE("OctTree: Inserting less than N elements into the tree")
{
    OctTree tree{vec3{-100, -100, -100}, vec3{100, 100, 100}};

    REQUIRE(tree.insert(vec3{0, 0, 0}));
}

TEST_CASE("OctTree: Subdivison")
{}
