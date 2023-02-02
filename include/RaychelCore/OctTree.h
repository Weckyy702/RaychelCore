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
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
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
    struct BasicBoundingBox
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

        template <typename Obj, auto Member>
            requires(std::is_member_pointer_v<decltype(Member)>)
        [[nodiscard]] constexpr auto& get_member(Obj& obj)
        {
            return std::invoke(Member, obj);
        }

        template <Coordinate Coord>
        constexpr auto get_x(const Coord& coord)
        {
            return get_member<Coord, &Coord::x>(coord);
        }

        template <Coordinate Coord>
        constexpr auto& get_x(Coord& coord)
        {
            return get_member<Coord, &Coord::x>(coord);
        }

        template <Coordinate Coord>
        constexpr auto get_y(const Coord& coord)
        {
            return get_member<Coord, &Coord::y>(coord);
        }

        template <Coordinate Coord>
        constexpr auto& get_y(Coord& coord)
        {
            return get_member<Coord, &Coord::y>(coord);
        }

        template <Coordinate Coord>
        constexpr auto get_z(const Coord& coord)
        {
            return get_member<Coord, &Coord::z>(coord);
        }

        template <Coordinate Coord>
        constexpr auto& get_z(Coord& coord)
        {
            return get_member<Coord, &Coord::z>(coord);
        }

        template <Coordinate Coord>
        constexpr Coord midpoint(const BasicBoundingBox<Coord> box)
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
        [[nodiscard]] constexpr bool contained_in(const Coord& coordinate, const BasicBoundingBox<Coord>& bounding_box)
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
            requires(Index < 8U)
        [[nodiscard]] constexpr BasicBoundingBox<Coord> bounding_box_for(
            const BasicBoundingBox<Coord>& bounding_box, const Coord& midpoint) noexcept
        {
            Coord min{bounding_box.bottom_front_left};
            Coord max{midpoint};

            if constexpr (Index & 1U) {
                get_x<Coord>(min) = get_x<Coord>(midpoint);
                get_x<Coord>(max) = get_x<Coord>(bounding_box.top_back_right);
            }

            if constexpr (Index & 2U) {
                get_y<Coord>(min) = get_y<Coord>(midpoint);
                get_y<Coord>(max) = get_y<Coord>(bounding_box.top_back_right);
            }

            if constexpr (Index & 4U) {
                get_z<Coord>(min) = get_z<Coord>(midpoint);
                get_z<Coord>(max) = get_z<Coord>(bounding_box.top_back_right);
            }

            return BasicBoundingBox<Coord>{min, max};
        }

        template <Coordinate Coord>
        [[nodiscard]] std::array<BasicBoundingBox<Coord>, 8>
        subdivide_bounding_box(const BasicBoundingBox<Coord>& bounding_box, const Coord& midpoint) noexcept
        {
            return {
                bounding_box_for<0>(bounding_box, midpoint),
                bounding_box_for<1>(bounding_box, midpoint),
                bounding_box_for<2>(bounding_box, midpoint),
                bounding_box_for<3>(bounding_box, midpoint),
                bounding_box_for<4>(bounding_box, midpoint),
                bounding_box_for<5>(bounding_box, midpoint),
                bounding_box_for<6>(bounding_box, midpoint),
                bounding_box_for<7>(bounding_box, midpoint),
            };
        }

        struct T2Coord
        {
            template <Coordinate T>
                requires std::copyable<T>
            [[nodiscard]] constexpr T operator()(const T& obj) const noexcept
            {
                return obj;
            }
        };

        template <Coordinate T>
        using ElementType = std::remove_cvref_t<std::invoke_result_t<decltype(&T::x), T>>;

        struct GetDistance
        {
            template <typename T>
            constexpr auto sq(T x) const noexcept
            {
                return x * x;
            }

            template <Coordinate Coord, typename ValueType = ElementType<Coord>>
                requires std::floating_point<ValueType>
            [[nodiscard]] constexpr ValueType operator()(const Coord& a, const Coord& b) const noexcept
            {
                return std::sqrt(sq(get_x(a) - get_x(b)) + sq(get_y(a) - get_y(b)) + sq(get_z(a) - get_z(b)));
            }
        };

        template <Coordinate Coord>
        struct ClosestItem
        {
            using ValueType = ElementType<Coord>;

            std::size_t index;
            ValueType distance;
        };

        template <std::size_t BucketSize, Coordinate Coordinate, typename GetDistance>
        class OctNode
        {
            using BoundingBox = BasicBoundingBox<Coordinate>;

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

                [[nodiscard]] constexpr auto begin() const noexcept
                {
                    return indecies_.begin();
                }

                [[nodiscard]] constexpr auto end() const noexcept
                {
                    return begin() + next_slot_;
                }

                ~IndeciesContainer() noexcept
                    requires(!std::is_trivially_destructible_v<Coordinate>)
                {
                    for (std::size_t i{}; i != BucketSize; ++i) {
                        _locations()[i].~Coordinate();
                    }
                }

                ~IndeciesContainer() noexcept
                    requires(std::is_trivially_destructible_v<Coordinate>)
                = default;

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
                {
                    children_.reserve(8U);

                    for (std::size_t i{}; i != 8; ++i) {
                        children_.emplace_back(std::move(bounding_boxes[i]));
                    }
                }

                [[nodiscard]] constexpr OctNode& operator[](std::size_t index) noexcept
                {
                    return children_[index];
                }

                [[nodiscard]] constexpr const OctNode& operator[](std::size_t index) const noexcept
                {
                    return children_[index];
                }

                [[nodiscard]] constexpr auto begin() const noexcept
                {
                    return children_.begin();
                }

                [[nodiscard]] constexpr auto end() const noexcept
                {
                    return children_.end();
                }

            private:
                //TODO: we use a vector even though the number of children is constant and known at compile time
                //Can't use an array tho beacuse the data structure is recursive
                std::vector<OctNode> children_{};
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
                ++size_;
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

            [[nodiscard]] constexpr OctNode& node_containing(const Coordinate& where) const noexcept
            {
                if (!has_children())
                    return *this;

                const auto& children = std::get<ChildrenContainer>(indecies_or_children_);
                const auto child_index = _child_index_for(where);

                return children[child_index].node_containing(where);
            }

            [[nodiscard]] constexpr std::size_t size() const noexcept
            {
                return size_;
            }

            constexpr void
            collect_closest(const Coordinate& where, std::optional<ClosestItem<Coordinate>>& maybe_closest_item) const noexcept
            {
                //Bail out if this node is empty
                if (size() == 0U)
                    return;

                if (!has_children()) {
                    _collect_own_indecies(where, maybe_closest_item);
                    return;
                }

                //Add only the children neighbouring that point
                const auto& children = std::get<ChildrenContainer>(indecies_or_children_);
                const auto [care_mask, side_mask] = _calculate_children_masks(where);

                for (std::size_t i{}; i != 8; ++i) {
                    //The index encodes a location. See _child_index_for

                    if (((i ^ side_mask) & care_mask) == care_mask) {
                        children[i].collect_closest(where, maybe_closest_item);
                    }
                }
            }

            void debug_print(std::size_t depth) const noexcept
            {
                if (size() == 0)
                    return;

                std::string indent(depth * 2, ' ');

                std::cerr << "Node{\n";
                std::cerr << indent << " BoundingBox={\n";
                std::cerr << indent << "  min={" << get_x(bounding_box_.bottom_front_left) << ", "
                          << get_y(bounding_box_.bottom_front_left) << ", " << get_z(bounding_box_.bottom_front_left) << "},\n";
                std::cerr << indent << "  max={" << get_x(bounding_box_.top_back_right) << ", "
                          << get_y(bounding_box_.top_back_right) << ", " << get_z(bounding_box_.top_back_right) << "}\n";
                std::cerr << indent << " },\n";

                if (!has_children()) {
                    std::cerr << indent << " Indecies={";

                    const auto& indecies = std::get<IndeciesContainer>(indecies_or_children_);
                    for (std::size_t i{}; i != BucketSize - 1; ++i) {
                        std::cerr << indecies.index_at(i) << ", ";
                    }
                    std::cerr << indecies.index_at(BucketSize - 1) << "}\n" << indent << "}\n";
                } else {
                    std::cerr << indent << " Children={\n";
                    const auto& children = std::get<ChildrenContainer>(indecies_or_children_);

                    std::size_t index{};
                    for (const auto& child : children) {
                        ++index;
                        if (child.size() == 0)
                            continue;
                        std::cerr << indent << ' ' << index - 1 << ": ";
                        child.debug_print(depth + 1);
                    }
                    std::cerr << indent << "}\n";
                }
            }

        private:
            constexpr void _collect_own_indecies(
                const Coordinate& where, std::optional<ClosestItem<Coordinate>>& maybe_closest_item) const noexcept
            {
                const auto& indecies = std::get<IndeciesContainer>(indecies_or_children_);

                for (std::size_t i{}; i != BucketSize; ++i) {
                    const auto coord = indecies.coord_at(i);
                    const auto index = indecies.index_at(i);

                    const auto distance = _get_distance(where, coord);

                    if (!maybe_closest_item.has_value()) [[unlikely]] {
                        maybe_closest_item.emplace(index, distance);
                        continue;
                    }

                    if (distance < maybe_closest_item->distance) {
                        maybe_closest_item->index = index;
                        maybe_closest_item->distance = distance;
                    }
                }
            }

            [[nodiscard]] constexpr std::size_t _child_index_for(Coordinate where)
            {
                //!!BITMAGIC!!
                // returns binary |00000000|...|00000abc|
                // where a = x-axis; b = y-axis; c = z-axis

                std::size_t index{};
                if (get_x<Coordinate>(where) > get_x<Coordinate>(midpoint_)) {
                    index |= 1U;
                }
                if (get_y<Coordinate>(where) > get_y<Coordinate>(midpoint_)) {
                    index |= 2U;
                }
                if (get_z<Coordinate>(where) > get_z<Coordinate>(midpoint_)) {
                    index |= 4U;
                }

                return index;
            }

            [[nodiscard]] constexpr auto _calculate_children_masks(const Coordinate& where) const noexcept
            {
                //!!BITMAGIC!!
                //returns two binary numbers, where the second one dictates if the point is inside or outside this cell with layout
                // 00000zyx
                //The first number indicates what side of the midpoint the point lies with 0 meaning less and layout
                // 00000zyx

                std::uint8_t care_mask{7};
                std::uint8_t side_mask{};

                if (in_range(get_x(where), get_x(bounding_box_.bottom_front_left), get_x(bounding_box_.top_back_right))) {
                    care_mask &= 0b110U;
                }
                if (in_range(get_y(where), get_y(bounding_box_.bottom_front_left), get_y(bounding_box_.top_back_right))) {
                    care_mask &= 0b101U;
                }
                if (in_range(get_z(where), get_z(bounding_box_.bottom_front_left), get_z(bounding_box_.top_back_right))) {
                    care_mask &= 0b011U;
                }

                if (get_x(where) <= get_x(midpoint_)) {
                    side_mask |= 1U;
                }
                if (get_y(where) <= get_y(midpoint_)) {
                    side_mask |= 2U;
                }
                if (get_z(where) <= get_z(midpoint_)) {
                    side_mask |= 4U;
                }

                return std::make_pair(care_mask, side_mask);
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
                // Create 8 children, each with its own subdivision of the bounding box
                ChildrenContainer children{subdivide_bounding_box(bounding_box_, midpoint_)};

                // Put the items into the children using their coordinates
                // TODO: try doing this in one step while initializing the children
                for (std::size_t i{}; i != BucketSize; ++i) {
                    const auto where = items.coord_at(i);
                    const auto index = _child_index_for(where);

                    children[index].insert(items.index_at(i), where);
                }

                indecies_or_children_ = std::move(children);
            }

            std::variant<IndeciesContainer, ChildrenContainer> indecies_or_children_{};

            BoundingBox bounding_box_;
            Coordinate midpoint_;
            std::size_t size_{};

            GetDistance _get_distance{};
        };
    } // namespace details

    template <Coordinate Coord>
    [[nodiscard]] constexpr BasicBoundingBox<Coord> make_bounding_box(const Coord& a, const Coord& b)
    {
        Coord min{
            std::min(details::get_x(a), details::get_x(b)),
            std::min(details::get_y(a), details::get_y(b)),
            std::min(details::get_z(a), details::get_z(b)),
        };
        Coord max{
            std::max(details::get_x(a), details::get_x(b)),
            std::max(details::get_y(a), details::get_y(b)),
            std::max(details::get_z(a), details::get_z(b)),
        };

        return BasicBoundingBox<Coord>{min, max};
    }

    template <
        typename T, std::size_t BucketSize = 10, Coordinate Coordinate = T, std::invocable<const T&> T2Coord = details::T2Coord,
        std::invocable<const Coordinate&, const Coordinate&> GetDistance = details::GetDistance>
        requires(std::is_invocable_r_v<Coordinate, T2Coord, const T&>) && std::copyable<T>
    class OcTree
    {
        using Node = details::OctNode<BucketSize, Coordinate, GetDistance>;
        using BoundingBox = BasicBoundingBox<Coordinate>;

        template <typename Ref, typename Dist>
        struct ClosestItem
        {
            Ref value;
            Dist distance;
        };

    public:
        constexpr explicit OcTree(const Coordinate& a, const Coordinate& b) : root_{make_bounding_box(a, b)}
        {}

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return elements_.size();
        }

        constexpr bool insert(T value)
        {
            const auto where = _t2coord(value);

            if (!details::contained_in(where, root_.bounding_box()))
                return false;

            elements_.push_back(std::move(value));
            root_.insert(elements_.size() - 1, where);

            return true;
        }

        constexpr auto begin() noexcept
        {
            return elements_.begin();
        }

        constexpr auto end() noexcept
        {
            return elements_.end();
        }

        constexpr auto cbegin() const noexcept
        {
            return elements_.cbegin();
        }

        constexpr auto cend() const noexcept
        {
            return elements_.cend();
        }

        [[nodiscard]] constexpr auto closest_to(const Coordinate& where) const noexcept
            -> std::optional<ClosestItem<const T&, details::ElementType<Coordinate>>>
        {
            if (size() == 0)
                return std::nullopt;

            std::optional<details::ClosestItem<Coordinate>> closest_item{};

            root_.collect_closest(where, closest_item);

            if (!closest_item.has_value())
                return std::nullopt;

            const auto [index, distance] = closest_item.value();

            return ClosestItem<const T&, details::ElementType<Coordinate>>{elements_[index], distance};
        }

        void debug_print() const noexcept
        {
            root_.debug_print(0U);
        }

    private:
        Node root_;
        std::vector<T> elements_{};
        T2Coord _t2coord{};
        GetDistance _get_distance{};
    };

} //namespace Raychel

#endif //!RAYCHELCORE_OCTTREE_H
