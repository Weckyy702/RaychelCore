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

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <numeric>
#include <type_traits>
#include <variant>
#include <vector>

namespace Raychel {
    // clang-format off
    template <typename T>
    concept Coordinate = requires(T t) {
        requires requires {
            { t.x } -> std::totally_ordered;
            { t.y } -> std::totally_ordered;
            { t.z } -> std::totally_ordered;
        } || requires {
            { t.x() } -> std::totally_ordered;
            { t.y() } -> std::totally_ordered;
            { t.z() } -> std::totally_ordered;
        };
        //TODO: T must be constructible from its three coordinates
   };
    // clang-format on

    template <Coordinate Coordinate>
    struct BoundingBox
    {
        Coordinate bottom_front_left, top_back_right;
    };

    namespace details {
        template <typename Obj, auto Member>
            requires(std::is_member_pointer_v<decltype(Member)>)
        [[nodiscard]] constexpr auto get_member(const Obj& obj)
        {
            return std::invoke(Member, obj);
        }

        template <Coordinate Coord>
        constexpr auto get_x = get_member<Coord, &Coord::x>;

        template <Coordinate Coord>
        constexpr auto get_y = get_member<Coord, &Coord::y>;

        template <Coordinate Coord>
        constexpr auto get_z = get_member<Coord, &Coord::z>;

        template <Coordinate Coord>
        constexpr Coord midpoint(const BoundingBox<Coord> box)
        {
            return Coord{
                std::midpoint(get_x<Coord>(box.bottom_front_left), get_x<Coord>(box.top_back_right)),
                std::midpoint(get_y<Coord>(box.bottom_front_left), get_y<Coord>(box.top_back_right)),
                std::midpoint(get_z<Coord>(box.bottom_front_left), get_z<Coord>(box.top_back_right)),
            };
        }

        template <std::totally_ordered T>
        constexpr bool in_range(const T& value, const T& min, const T& max)
        {
            return (value >= min) && (value <= max);
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr bool contained_in(const Coord& coordinate, const BoundingBox<Coord>& bounding_box)
        {
            return in_range(
                       get_x<Coord>(coordinate),
                       get_x<Coord>(bounding_box.bottom_front_left),
                       get_x<Coord>(bounding_box.top_back_right)) &&
                   in_range(
                       get_y<Coord>(coordinate),
                       get_y<Coord>(bounding_box.bottom_front_left),
                       get_y<Coord>(bounding_box.top_back_right)) &&
                   in_range(
                       get_z<Coord>(coordinate),
                       get_z<Coord>(bounding_box.bottom_front_left),
                       get_z<Coord>(bounding_box.top_back_right));
        }

        template <std::size_t Index, Coordinate Coord>
        [[nodiscard]] constexpr BoundingBox<Coord>
        bounding_box_for(const BoundingBox<Coord>& bounding_box, const Coord& midpoint) noexcept
        {
            //TODO: implement
            assert(false);

            BoundingBox<Coord> res{Coord{0, 0, 0}, Coord{0, 0, 0}};

            if (Index & 1U) {
            }

            if (Index & 2U) {
            }

            if (Index & 4U) {
            }

            return res;
        }

        template <Coordinate Coord>
        [[nodiscard]] std::array<BoundingBox<Coord>, 8>
        subdivide_bounding_box(const BoundingBox<Coord>& bounding_box, const Coord& midpoint) noexcept
        {
            assert(false);
            return {};
        }

        struct T2Coord
        {
            template <std::copyable T>
            [[nodiscard]] constexpr T operator()(const T& obj) noexcept
            {
                return obj;
            }
        };

        template <std::size_t BucketSize, Coordinate Coordinate>
        class OctNode
        {
            using BoundingBox = BoundingBox<Coordinate>;

            class IndeciesContainer
            {
            public:
                [[nodiscard]] constexpr bool is_full() const noexcept
                {
                    return next_slot_ == BucketSize;
                }

                constexpr void insert(std::size_t index_in_tree, const Coordinate& where) noexcept
                {
                    indecies_[next_slot_] = index_in_tree;
                    new (_locations() + next_slot_) Coordinate{where};

                    ++next_slot_;
                }

                [[nodiscard]] constexpr Coordinate coord_at(std::size_t index) const noexcept
                {
                    return _locations()[index];
                }

                [[nodiscard]] constexpr std::size_t index_at(std::size_t index) const noexcept
                {
                    return indecies_[index];
                }

            private:
                [[nodiscard]] constexpr const Coordinate* _locations() const noexcept
                {
                    return reinterpret_cast<const Coordinate*>(&index_data);
                }

                [[nodiscard]] constexpr Coordinate* _locations() noexcept
                {
                    return reinterpret_cast<Coordinate*>(&index_data);
                }

                std::array<std::size_t, BucketSize> indecies_;
                std::aligned_storage_t<sizeof(Coordinate) * BucketSize> index_data{};
                std::size_t next_slot_{};
            };

            class ChildrenContainer
            {
                using Children = OctNode[];

            public:
                constexpr explicit ChildrenContainer(std::array<BoundingBox, 8> bounding_boxes)
                    : children_{reinterpret_cast<OctNode*>(::operator new(sizeof(OctNode) * 8U))}
                {
                    for (std::size_t i{}; i != 8; ++i) {
                        new (children_.get() + i) OctNode{bounding_boxes[i]};
                    }
                }

                [[nodiscard]] constexpr OctNode& operator[](std::size_t index) noexcept
                {
                    return children_[index];
                }

            private:
                std::unique_ptr<Children> children_;
            };

        public:
            explicit constexpr OctNode(BoundingBox bounding_box)
                : bounding_box_{std::move(bounding_box)}, midpoint_{midpoint(bounding_box_)}
            {}

            [[nodiscard]] constexpr BoundingBox bounding_box() const noexcept
            {
                return bounding_box_;
            }

            [[nodiscard]] constexpr bool has_children() const noexcept
            {
                return indecies_or_children_.index() == 1U;
            }

            constexpr void insert(std::size_t index_in_tree, const Coordinate& where) noexcept
            {
                (void)where;
                if (has_children()) {
                    _insert_into_children(index_in_tree, where);
                    return;
                }

                IndeciesContainer& bucket = std::get<IndeciesContainer>(indecies_or_children_);

                if (bucket.is_full()) {
                    //Subdivide and insert into children
                    _subdivide();
                    _insert_into_children(index_in_tree, where);
                    return;
                }

                bucket.insert(index_in_tree, where);
            }

        private:
            [[nodiscard]] constexpr std::size_t _child_index_for(Coordinate where)
            {
                //!!BITMAGIC!!
                // returns binary |00000000|...|00000abc|
                // where a = x-axis; b = y-axis; c = z-axis

                std::size_t index{};
                if (get_x<Coordinate>(where) >= get_x<Coordinate>(midpoint_)) {
                    index |= 1U;
                }
                if (get_y<Coordinate>(where) >= get_y<Coordinate>(midpoint_)) {
                    index |= 2U;
                }
                if (get_z<Coordinate>(where) >= get_z<Coordinate>(midpoint_)) {
                    index |= 4U;
                }

                return index;
            }

            constexpr void _insert_into_children(std::size_t index_in_tree, const Coordinate& where) noexcept
            {
                const auto child_index = _child_index_for(where);
                auto& children = std::get<ChildrenContainer>(indecies_or_children_);

                children[child_index].insert(index_in_tree, where);
            }

            constexpr void _subdivide() noexcept
            {
                //Save current items
                auto items = std::get<IndeciesContainer>(std::move(indecies_or_children_));

                //Create children
                // Create 8 children, each with its own subdivision of the bounding box and
                [[maybe_unused]] auto& children =
                    indecies_or_children_.template emplace<ChildrenContainer>(subdivide_bounding_box(bounding_box_, midpoint_));

                // Put the items into the children using their coordinates
                // TODO: try doing this in one step while initializing the children
                for (std::size_t i{}; i != BucketSize; ++i) {
                    const auto where = items.coord_at(i);
                    const auto index = _child_index_for(where);

                    children[index].insert(items.index_at(i), where);
                }
            }

            std::variant<IndeciesContainer, ChildrenContainer> indecies_or_children_{};

            BoundingBox bounding_box_;
            Coordinate midpoint_;
        };
    } // namespace details

    template <
        typename T, std::size_t BucketSize = 10, Coordinate Coordinate = T, std::invocable<const T&> T2Coord = details::T2Coord>
        requires(std::is_invocable_r_v<Coordinate, T2Coord, const T&>)
    class OctTree
    {
        using Node = details::OctNode<BucketSize, Coordinate>;
        using BoundingBox = BoundingBox<Coordinate>;

    public:
        constexpr explicit OctTree(BoundingBox bounding_box) : root_{bounding_box}
        {}

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return points_.size();
        }

        constexpr bool insert(T value)
        {
            const auto where = _t2coord(value);

            if (!details::contained_in(where, root_.bounding_box()))
                return false;

            points_.push_back(std::move(value));
            root_.insert(points_.size() - 1, where);

            return true;
        }

        constexpr auto begin() noexcept
        {
            return points_.begin();
        }

        constexpr auto end() noexcept
        {
            return points_.end();
        }

        constexpr auto cbegin() const noexcept
        {
            return points_.cbegin();
        }

        constexpr auto cend() const noexcept
        {
            return points_.cend();
        }

    private:
        Node root_;
        std::vector<T> points_{};
        T2Coord _t2coord{};
    };

} //namespace Raychel

#endif //!RAYCHELCORE_OCTTREE_H
