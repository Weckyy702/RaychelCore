#include "RaychelCore/OctTree.h"

#include "catch2/catch.hpp"

#include <chrono>
#include <cstddef>
#include <memory_resource>
#include <numbers>
#include <ostream>
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

std::ostream& operator<<(std::ostream& os, const vec3& v) noexcept
{
    return os << '{' << v.x << ", " << v.y << ", " << v.z << '}';
}

struct Triangle
{
    [[nodiscard]] vec3 midpoint() const noexcept
    {
        return vec3{(a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3};
    }

    constexpr auto operator<=>(const Triangle& other) const noexcept = default;

    vec3 a, b, c;
};

std::ostream& operator<<(std::ostream& os, const Triangle& triangle) noexcept
{
    return os << "{\n  " << triangle.a << ",\n  " << triangle.b << ",\n  " << triangle.c << "\n}";
}

struct TriangleDistance
{
    double operator()(const Triangle& triangle, const vec3& v) const noexcept
    {
        return Raychel::details::GetDistanceToPoint{}(triangle.midpoint(), v);
    }
};

struct TriangleBoundingBox
{
    Raychel::BasicBoundingBox<vec3> operator()(const Triangle& triangle) const noexcept
    {
        return Raychel::BasicBoundingBox<vec3>{
            .bottom_front_left =
                vec3{
                    std::min({triangle.a.x, triangle.b.x, triangle.c.x}),
                    std::min({triangle.a.y, triangle.b.y, triangle.c.y}),
                    std::min({triangle.a.z, triangle.b.z, triangle.c.z}),
                },
            .top_back_right = vec3{
                std::max({triangle.a.x, triangle.b.x, triangle.c.x}),
                std::max({triangle.a.y, triangle.b.y, triangle.c.y}),
                std::max({triangle.a.z, triangle.b.z, triangle.c.z}),
            }};
    }
};

using OctTree = Raychel::OcTree<vec3, 10, 5>;

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
    OctTree tree{vec3{0, 0, 0}, vec3{100, 100, 100}};

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

        for (std::size_t i{}; i != 1'000; ++i) {
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

static vec3 ngon_point(int point_index, double num_points)
{
    const auto angle = static_cast<double>(point_index) * 2 * std::numbers::pi / num_points;
    return vec3{50 + 25 * std::sin(angle), 50 + 25 * std::cos(angle), 0};
}

static Triangle ngon_triangle(int point_index, double num_points)
{
    return Triangle{
        ngon_point(point_index - 1, num_points),
        ngon_point(point_index, num_points),
        ngon_point(point_index + 1, num_points),
    };
}

TEST_CASE("OcTree: Ts with bounding boxes")
{
    using TriangleTree = Raychel::OcTree<Triangle, 10, 2, vec3, TriangleBoundingBox, TriangleDistance>;

    SECTION("Triangles sharing points")
    {
        TriangleTree tree{vec3{0, 0, 0}, vec3{100, 100, 100}};
        for (int i{}; i != 7; ++i) {
            REQUIRE(tree.insert(ngon_triangle(i, 7)));
            REQUIRE(tree.size() == static_cast<std::size_t>(i) + 1);
        }
    }

    SECTION("Get closest neighbor")
    {
        TriangleTree tree{vec3{0, 0, 0}, vec3{100, 100, 100}};

        REQUIRE(tree.size() == 0);

        std::mt19937 rng{12334};
        std::uniform_real_distribution dist(0., 100.);

        double min_distance{1e9};
        Triangle closest{vec3{100, 100, 100}, vec3{200, 200, 200}, vec3{300, 300, 300}};

        for (std::size_t i{}; i != 1'000; ++i) {
            Triangle obj{
                vec3{dist(rng), dist(rng), dist(rng)},
                vec3{dist(rng), dist(rng), dist(rng)},
                vec3{dist(rng), dist(rng), dist(rng)},
            };

            const auto distance = TriangleDistance{}(obj, vec3{50, 50, 50});
            if (distance < min_distance) {
                min_distance = distance;
                closest = obj;
            }

            REQUIRE(tree.insert(obj));
            REQUIRE(tree.size() == i + 1);
        }

        tree.debug_print();

        const auto maybe_value = tree.closest_to(vec3{50, 50, 50});
        REQUIRE(maybe_value.has_value());

        const auto [object, distance] = maybe_value.value();

        REQUIRE(distance == min_distance);
        REQUIRE(object == closest);
    }
}
