/**
* \file OctTree.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for OctTree class
* \date 2023-01-30
*
* MIT License
* Copyright (c) [2023] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/
#ifndef RAYCHELCORE_OCTTREE_H
#define RAYCHELCORE_OCTTREE_H

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace Raychel {

    namespace details {

        // clang-format off

        template<typename T>
        concept InvocableCoordinate = requires(T t) {
            { t.x() } -> ::std::totally_ordered;
            { t.y() } -> ::std::totally_ordered;
            { t.z() } -> ::std::totally_ordered;
        };

        template<typename T>
        concept MemberCoordinate = requires(T t) {
            { t.x } -> ::std::totally_ordered;
            { t.y } -> ::std::totally_ordered;
            { t.z } -> ::std::totally_ordered;
        };

        template<typename T>
        concept Coordinate = InvocableCoordinate<T> || MemberCoordinate<T>;
        // clang-format on

        template <std::totally_ordered T>
        constexpr bool in_range(T value, T min, T max)
        {
            return (value >= min) && (value < max);
        }

        template <MemberCoordinate Coord>
        constexpr bool contained_in(const Coord& coord, const Coord& top_left, const Coord& bottom_right)
        {
            return in_range(coord.x, top_left.x, bottom_right.x) && in_range(coord.y, top_left.y, bottom_right.y) &&
                   in_range(coord.z, top_left.z, bottom_right.z);
        }

        template <InvocableCoordinate Coord>
        constexpr bool contained_in(const Coord& coord, const Coord& top_left, const Coord& bottom_right)
        {
            return in_range(coord.x(), top_left.x(), bottom_right.x()) && in_range(coord.y(), top_left.y(), bottom_right.y()) &&
                   in_range(coord.z(), top_left.z(), bottom_right.z());
        }

        struct ContainedIn
        {
            template <Coordinate T>
            constexpr bool operator()(const T& coord, const T& top_left, const T& bottom_right)
            {
                return contained_in(coord, top_left, bottom_right);
            }
        };

        template <
            typename T, std::size_t BucketCapacity, Coordinate Coord,
            std::invocable<const Coord&, const Coord&, const Coord&> ContainedIn>
        class OctNode
        {

            struct Bucket
            {
                std::array<T, BucketCapacity> items;
                std::size_t current_index{};
            };

            struct ChildrenContainer
            {
                struct Child
                {
                    std::size_t index_in_parent;
                    OctNode child;
                };

                std::vector<std::optional<Child>> children;
            };

        public:
            OctNode(Coord top_back_left, Coord bottom_front_right) noexcept(std::is_nothrow_move_constructible_v<Coord>)
                : elements_or_children_{Bucket{}},
                  top_left_corner_{std::move(top_back_left)},
                  bottom_right_corner_{std::move(bottom_front_right)}
            {}

            [[nodiscard]] constexpr bool has_children() const noexcept
            {
                return elements_or_children_.index() == 1U;
            }

            [[nodiscard]] constexpr bool insert(T value) //TODO: exception specification
            {
                (void)value;
                return false;
            }

        private:
            constexpr void _divide() noexcept
            {
                //Noop if we are already divided (shouldn't happen?)
                if (has_children()) {
                    return;
                }

                //Create children
                // Divide bounds into eight smaller subregions
                // Create new OctNode for each subregion

                //Sort item into children
                // For every item
                //  Calculate which child it belongs into
                //  Put it into that node

                //Disgard empty children? (They use quite a bit of memory)
            }

            constexpr auto _elements() noexcept
            {
                return std::get<0>(std::move(elements_or_children_));
            }

            std::variant<Bucket, ChildrenContainer> elements_or_children_;
            Coord top_left_corner_;
            Coord bottom_right_corner_;

            ContainedIn _contained_in{};
        };
    }; // namespace details

    template <typename T, std::size_t BucketCapacity, typename Coordinates, typename ContainedIn = details::ContainedIn>
    class OctTree
    {
        using Node = details::OctNode<T, BucketCapacity, Coordinates, ContainedIn>;

    public:
        OctTree(Coordinates top_back_left, Coordinates bottom_front_right)
            : root_{std::move(top_back_left), std::move(bottom_front_right)}
        {}

        [[nodiscard]] constexpr std::size_t size()
        {
            return size_;
        }

        constexpr bool insert(T value)
        {
            return root_.insert(std::move(value));
        }

    private:
        Node root_;
        std::size_t size_{};

        ContainedIn _contained_in{};
    };
} //namespace Raychel

#endif //!RAYCHELCORE_OCTTREE_H
