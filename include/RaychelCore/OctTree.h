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
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
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
            return (value >= min) && (value <= max);
        }

        template <typename Object, auto Member>
        constexpr auto get(const Object& obj)
        {
            return std::invoke(Member, obj);
        }

        template <Coordinate Object>
        constexpr auto get_x = get<Object, &Object::x>;

        template <Coordinate Object>
        constexpr auto get_y = get<Object, &Object::y>;

        template <Coordinate Object>
        constexpr auto get_z = get<Object, &Object::z>;

        template <Coordinate C, Coordinate BB>
        constexpr bool contained_in(const C& coord, const BB& top_left, const BB& bottom_right)
        {
            return in_range(get_x<C>(coord), get_x<BB>(top_left), get_x<BB>(bottom_right)) &&
                   in_range(get_y<C>(coord), get_y<BB>(top_left), get_y<BB>(bottom_right)) &&
                   in_range(get_z<C>(coord), get_z<BB>(top_left), get_z<BB>(bottom_right));
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
            std::invocable<const T&, const Coord&, const Coord&> ContainedIn>
        class OctNode
        {

            class Bucket
            {
            public:
                [[nodiscard]] constexpr bool is_full() const noexcept
                {
                    return next_index_ == BucketCapacity;
                }

                constexpr void insert(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
                {
                    if (is_full())
                        return;

                    new (_current()) T{std::move(value)};
                    ++next_index_;
                }

            private:
                constexpr T* _current() noexcept
                {
                    return _items() + next_index_;
                }

                constexpr T* _items()
                {
                    return reinterpret_cast<T*>(&storage_);
                }

                std::size_t next_index_{};
                std::aligned_storage_t<sizeof(T) * BucketCapacity, alignof(T)> storage_{};
            };

            class ChildrenContainer
            {
                struct Child
                {
                    std::size_t index_in_parent;
                    OctNode child;
                };

                using Children = Child[];

            public:
                explicit ChildrenContainer() : children_{std::make_unique<Children>(8)}
                {}

            private:
                std::unique_ptr<Children> children_;
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

            [[nodiscard]] constexpr bool insert(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
            {
                if (!_contained_in(value, top_left_corner_, bottom_right_corner_)) {
                    return false;
                }

                if (has_children()) {
                    _insert_into_children(std::move(value));
                } else {
                    _insert_into_bucket(std::move(value));
                }

                return true;
            }

        private:
            constexpr void _insert_into_bucket(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
            {
                Bucket& bucket = std::get<Bucket>(elements_or_children_);

                if (bucket.is_full()) {
                    //TODO
                    assert(false);
                    return;
                }

                bucket.insert(std::move(value));
            }

            constexpr void _insert_into_children(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
            {
                //TODO
                (void)value;
                assert(false);
            }

            constexpr void _subdivide() noexcept
            {
                //Noop if we are already divided (shouldn't happen?)
                if (has_children()) {
                    return;
                }

                //Create children
                // Divide bounds into eight smaller subregions
                // Create new OctNode for each subregion

                //Sort items into children
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
            if (root_.insert(std::move(value))) {
                ++size_;
                return true;
            }
            return false;
        }

    private:
        Node root_;
        std::size_t size_{};

        ContainedIn _contained_in{};
    };
} //namespace Raychel

#endif //!RAYCHELCORE_OCTTREE_H
