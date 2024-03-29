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

#include "RaychelCore/ClassMacros.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
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
    template<typename T>
    concept MemberCoordinate = requires(T t)
    {
        { t.x } -> std::totally_ordered;
        { t.y } -> std::totally_ordered;
        { t.z } -> std::totally_ordered;
    };

    template<typename T>
    concept InvokeCoordinate = requires(T t)
    {
        { t.x() } -> std::totally_ordered;
        { t.y() } -> std::totally_ordered;
        { t.z() } -> std::totally_ordered;
    };

    template <typename T>
    concept Coordinate = MemberCoordinate<T> || InvokeCoordinate<T>;
    // clang-format on

    template <Coordinate Coordinate>
    struct BasicBoundingBox
    {
        Coordinate bottom_front_left, top_back_right;
    };

    namespace details {

        template <MemberCoordinate T>
        constexpr auto& get_x(T& obj)
        {
            return obj.x;
        }

        template <MemberCoordinate T>
        constexpr const auto& get_x(const T& obj)
        {
            return obj.x;
        }

        template <MemberCoordinate T>
        constexpr auto& get_y(T& obj)
        {
            return obj.y;
        }

        template <MemberCoordinate T>
        constexpr const auto& get_y(const T& obj)
        {
            return obj.y;
        }

        template <MemberCoordinate T>
        constexpr auto& get_z(T& obj)
        {
            return obj.z;
        }

        template <MemberCoordinate T>
        constexpr const auto& get_z(const T& obj)
        {
            return obj.z;
        }

        template <InvokeCoordinate T>
        constexpr auto& get_x(T& obj)
        {
            return obj.x();
        }

        template <InvokeCoordinate T>
        constexpr const auto& get_x(const T& obj)
        {
            return obj.x();
        }

        template <InvokeCoordinate T>
        constexpr auto& get_y(T& obj)
        {
            return obj.y();
        }

        template <InvokeCoordinate T>
        constexpr const auto& get_y(const T& obj)
        {
            return obj.y();
        }

        template <InvokeCoordinate T>
        constexpr auto& get_z(T& obj)
        {
            return obj.z();
        }

        template <InvokeCoordinate T>
        constexpr const auto& get_z(const T& obj)
        {
            return obj.z();
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

        template <std::size_t CornerIndex, Coordinate Coord>
            requires(std::copy_constructible<Coord> && (CornerIndex < 8U))
        constexpr Coord get_corner(const BasicBoundingBox<Coord>& b)
        {
            Coord res{b.bottom_front_left};

            if constexpr (CornerIndex & 1U) {
                get_x(res) = get_x(b.top_back_right);
            }
            if constexpr (CornerIndex & 2U) {
                get_y(res) = get_y(b.top_back_right);
            }
            if constexpr (CornerIndex & 4U) {
                get_z(res) = get_z(b.top_back_right);
            }

            return res;
        }

        template <std::totally_ordered T>
        constexpr bool in_range(const T& value, const T& min, const T& max)
        {
            return (min <= value) && (value <= max);
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr bool contains(const BasicBoundingBox<Coord>& bb, const Coord& c)
        {
            return in_range(get_x(c), get_x(bb.bottom_front_left), get_x(bb.top_back_right)) &&
                   in_range(get_y(c), get_y(bb.bottom_front_left), get_y(bb.top_back_right)) &&
                   in_range(get_z(c), get_z(bb.bottom_front_left), get_z(bb.top_back_right));
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr bool overlaps_part(const BasicBoundingBox<Coord>& a, const BasicBoundingBox<Coord>& b)
        {
            //TODO: can overlapping tests be done more efficiently with some bitwise magic?
            return contains(b, get_corner<0>(a)) || contains(b, get_corner<1>(a)) || contains(b, get_corner<2>(a)) ||
                   contains(b, get_corner<3>(a)) || contains(b, get_corner<4>(a)) || contains(b, get_corner<5>(a)) ||
                   contains(b, get_corner<6>(a)) || contains(b, get_corner<7>(a));
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr bool overlaps(const BasicBoundingBox<Coord>& a, const BasicBoundingBox<Coord>& b)
        {
            //TODO: this is really slow :(
            return overlaps_part(a, b) || overlaps_part(b, a);
        }

        template <std::size_t ChildIndex, Coordinate Coord>
            requires(ChildIndex < 8U)
        [[nodiscard]] constexpr BasicBoundingBox<Coord> bounding_box_for(
            const BasicBoundingBox<Coord>& bounding_box, const Coord& midpoint) noexcept
        {
            Coord min{bounding_box.bottom_front_left};
            Coord max{midpoint};

            if constexpr (ChildIndex & 1U) {
                get_x<Coord>(min) = get_x<Coord>(midpoint);
                get_x<Coord>(max) = get_x<Coord>(bounding_box.top_back_right);
            }

            if constexpr (ChildIndex & 2U) {
                get_y<Coord>(min) = get_y<Coord>(midpoint);
                get_y<Coord>(max) = get_y<Coord>(bounding_box.top_back_right);
            }

            if constexpr (ChildIndex & 4U) {
                get_z<Coord>(min) = get_z<Coord>(midpoint);
                get_z<Coord>(max) = get_z<Coord>(bounding_box.top_back_right);
            }

            return BasicBoundingBox<Coord>{min, max};
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr std::array<BasicBoundingBox<Coord>, 8>
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

        struct BoundingBoxFromCoordinate
        {
            template <Coordinate T>
                requires std::copyable<T>
            [[nodiscard]] constexpr BasicBoundingBox<T> operator()(const T& obj) const noexcept
            {
                return BasicBoundingBox<T>{obj, obj};
            }
        };

        template <Coordinate T>
        using ElementType = std::remove_cvref_t<decltype(get_x<T>(std::declval<const T&>()))>;

        template <typename T>
        [[nodiscard]] constexpr T sq(T x)
        {
            return x * x;
        }

        template <Coordinate Coord>
        [[nodiscard]] constexpr auto distance_squared(const Coord& a, const Coord& b)
        {
            return sq(get_x(a) - get_x(b)) + sq(get_y(a) - get_y(b)) + sq(get_z(a) - get_z(b));
        }

        struct GetDistanceToPoint
        {
            template <Coordinate Coord, typename ValueType = ElementType<Coord>>
                requires std::floating_point<ValueType>
            [[nodiscard]] constexpr ValueType operator()(const Coord& a, const Coord& b) const noexcept
            {
                return std::sqrt(distance_squared(a, b));
            }
        };

        template <Coordinate Coord>
        struct ClosestItem
        {
            using ValueType = ElementType<Coord>;

            explicit constexpr ClosestItem(std::size_t _index, ValueType _distance) : index{_index}, distance{_distance}
            {}

            std::size_t index;
            ValueType distance;
        };

        template <std::size_t BucketSize, std::size_t MaxDepth, Coordinate Coordinate, typename GetDistance, typename Tree>
        class OctNode
        {
            using BoundingBox = BasicBoundingBox<Coordinate>;
            using Number = ElementType<Coordinate>;

            class IndexContainer
            {
            public:
                constexpr IndexContainer() = default;

                [[nodiscard]] constexpr bool is_full() const noexcept
                {
                    return indecies_.size() >= BucketSize;
                }

                [[nodiscard]] constexpr std::size_t size() const noexcept
                {
                    return indecies_.size();
                }

                constexpr void insert(std::size_t index_in_tree, const BoundingBox& where) noexcept
                {
                    indecies_.push_back(index_in_tree);
                    bounding_boxes_.push_back(where);
                }

                [[nodiscard]] constexpr BoundingBox bounding_box_at(std::size_t index) const noexcept
                {
                    return bounding_boxes_[index];
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
                    return indecies_.end();
                }

            private:
                std::vector<std::size_t> indecies_{};
                std::vector<BoundingBox> bounding_boxes_{};
            };

            class ChildContainer
            {
            public:
                constexpr explicit ChildContainer(
                    std::array<BoundingBox, 8> bounding_boxes, Tree* parent, std::size_t children_depth)
                {
                    nodes_.reserve(8U);
                    for (std::size_t i{}; i != 8; ++i) {
                        nodes_.emplace_back(bounding_boxes[i], parent, children_depth);
                    }
                }

                [[nodiscard]] constexpr OctNode& operator[](std::size_t i) noexcept
                {
                    return nodes_[i];
                }

                [[nodiscard]] constexpr const OctNode& operator[](std::size_t i) const noexcept
                {
                    return nodes_[i];
                }

                [[nodiscard]] constexpr auto begin() noexcept
                {
                    return nodes_.begin();
                }

                [[nodiscard]] constexpr auto begin() const noexcept
                {
                    return nodes_.begin();
                }

                [[nodiscard]] constexpr auto end() noexcept
                {
                    return nodes_.end();
                }

                [[nodiscard]] constexpr auto end() const noexcept
                {
                    return nodes_.end();
                }

            private:
                std::vector<OctNode> nodes_{};
            };

        public:
            explicit constexpr OctNode(BoundingBox bounding_box, Tree* parent, std::size_t depth)
                : tree_{parent},
                  bounding_box_{std::move(bounding_box)},
                  midpoint_{midpoint(bounding_box_)},
                  radius_squared_{std::max({
                      sq(get_x(bounding_box_.top_back_right) - get_x(bounding_box_.bottom_front_left)),
                      sq(get_y(bounding_box_.top_back_right) - get_y(bounding_box_.bottom_front_left)),
                      sq(get_z(bounding_box_.top_back_right) - get_z(bounding_box_.bottom_front_left)),
                  })},
                  depth_{depth}
            {}

            [[nodiscard]] constexpr BoundingBox bounding_box() const noexcept
            {
                return bounding_box_;
            }

            [[nodiscard]] constexpr bool has_children() const noexcept
            {
                return std::holds_alternative<ChildContainer>(indecies_or_children_);
            }

            constexpr void insert(std::size_t index_in_tree, const BoundingBox& where) noexcept
            {
                ++size_;

                if (has_children()) {
                    _insert_into_children(index_in_tree, where);
                    return;
                }

                IndexContainer& bucket = std::get<IndexContainer>(indecies_or_children_);

                if (bucket.is_full() && depth_ < MaxDepth) {
                    _subdivide();
                    _insert_into_children(index_in_tree, where);
                    return;
                }

                bucket.insert(index_in_tree, where);
            }

            [[nodiscard]] constexpr std::size_t size() const noexcept
            {
                return size_;
            }

            constexpr void
            find_closest(const Coordinate& where, std::optional<ClosestItem<Coordinate>>& maybe_closest_item) const noexcept
            {
                //Bail out if this node is empty
                if (size() == 0U) [[unlikely]]
                    return;

                if (!has_children()) {
                    _find_closest(where, maybe_closest_item);
                    return;
                }

                const auto& children = std::get<ChildContainer>(indecies_or_children_);

                for (const auto& child : children) {
                    const auto child_distance_squared = distance_squared(child.midpoint_, where);
                    if (child_distance_squared <= radius_squared_) [[unlikely]] {
                        child.find_closest(where, maybe_closest_item);
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
                    const auto& indecies = std::get<IndexContainer>(indecies_or_children_);
                    if (indecies.size() != 0U) {
                        for (std::size_t i{}; i != indecies.size() - 1; ++i) {
                            std::cerr << indecies.index_at(i) << ", ";
                        }
                        std::cerr << indecies.index_at(indecies.size() - 1);
                    }
                    std::cout << "}\n";
                } else {
                    std::cerr << indent << " Children={\n";
                    const auto& children = std::get<ChildContainer>(indecies_or_children_);

                    std::size_t index{};
                    for (const auto& child : children) {
                        ++index;
                        if (child.size() == 0)
                            continue;
                        std::cerr << indent << ' ' << index - 1 << ": ";
                        child.debug_print(depth + 1);
                    }
                }
                std::cout << indent << "}\n";
            }

            constexpr void update_parent(Tree* parent) noexcept
            {
                tree_ = parent;

                if (!has_children()) {
                    return;
                }

                auto& children = std::get<ChildContainer>(indecies_or_children_);
                for (auto& child : children) {
                    child.update_parent(parent);
                }
            }

        private:
            constexpr void
            _find_closest(const Coordinate& where, std::optional<ClosestItem<Coordinate>>& maybe_closest_item) const noexcept
            {
                const auto& indecies = std::get<IndexContainer>(indecies_or_children_);

                for (const auto index : indecies) {
                    const auto distance = _get_distance(tree_->elements_[index], where);

                    if (!maybe_closest_item.has_value()) [[unlikely]] {
                        maybe_closest_item.emplace(index, distance);
                        continue;
                    }

                    if (distance < maybe_closest_item->distance) [[unlikely]] {
                        maybe_closest_item->index = index;
                        maybe_closest_item->distance = distance;
                    }
                }
            }

            constexpr void _insert_into_children(std::size_t index_in_tree, const BoundingBox& where) noexcept
            {
                auto& children = std::get<ChildContainer>(indecies_or_children_);

                for (auto& child : children) {
                    if (overlaps(where, child.bounding_box_))
                        child.insert(index_in_tree, where);
                }
            }

            constexpr void _subdivide() noexcept
            {
                //Save current items
                auto items = std::get<IndexContainer>(std::move(indecies_or_children_));

                //Create children
                // Create 8 children, each with its own subdivision of the bounding box
                ChildContainer children{subdivide_bounding_box(bounding_box_, midpoint_), tree_, depth_ + 1};

                // Put the items into the children using their coordinates
                // TODO: try doing this in one step while initializing the children
                for (std::size_t i{}; i != items.size(); ++i) {
                    const auto& where = items.bounding_box_at(i);

                    for (auto& child : children) {
                        if (overlaps(where, child.bounding_box_))
                            child.insert(items.index_at(i), where);
                    }
                }

                indecies_or_children_.template emplace<ChildContainer>(std::move(children));
            }

            std::variant<IndexContainer, ChildContainer> indecies_or_children_{};
            Tree* tree_;

            BoundingBox bounding_box_;
            Coordinate midpoint_;
            Number radius_squared_;

            std::size_t size_{};
            std::size_t depth_{};

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
        typename T, std::size_t BucketSize = 10, std::size_t MaxDepth = 20, Coordinate Coordinate = T,
        std::invocable<const T&> GetBoundingBox = details::BoundingBoxFromCoordinate,
        std::invocable<const T&, const Coordinate&> GetDistance = details::GetDistanceToPoint>
        requires(std::is_invocable_r_v<BasicBoundingBox<Coordinate>, GetBoundingBox, const T&>) && std::copyable<T>
    class OcTree
    {
        using Node = details::OctNode<BucketSize, MaxDepth, Coordinate, GetDistance, OcTree>;
        using BoundingBox = BasicBoundingBox<Coordinate>;

        friend Node;

        template <typename Ref, typename Dist>
        struct ClosestItem
        {
            Ref value;
            Dist distance;
        };

    public:
        constexpr OcTree(const Coordinate& a, const Coordinate& b, std::vector<T> items = {})
            : root_{make_bounding_box(a, b), this, 0U}, elements_(std::move(items))
        {
            _build_from_items();
        }

        constexpr explicit OcTree(std::pair<Coordinate, Coordinate> bounding_box, std::vector<T> items = {})
            : OcTree{bounding_box.first, bounding_box.second, std::move(items)}
        {}

        constexpr OcTree(const OcTree& other) : root_{other.root_}, elements_{other.elements_}
        {
            _root().update_parent(this);
        }

        constexpr OcTree& operator=(const OcTree& other)
        {
            root_ = other.root_;
            elements_ = other.elements_;

            _root().update_parent(this);

            return *this;
        }

        constexpr OcTree(OcTree&& other) noexcept : root_{std::move(other.root_)}, elements_{std::move(other.elements_)}
        {
            _root().update_parent(this);
        }

        constexpr OcTree& operator=(OcTree&& other) noexcept
        {
            root_ = std::move(other.root_);
            elements_ = std::move(other.elements_);

            _root().update_parent(this);

            return *this;
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return elements_.size();
        }

        constexpr bool insert(T value)
        {
            const auto where = _get_bounding_box(value);

            if (!details::overlaps(where, _root().bounding_box()))
                return false;

            elements_.push_back(std::move(value));
            _root().insert(elements_.size() - 1, where);

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

        constexpr auto begin() const noexcept
        {
            return elements_.begin();
        }

        constexpr auto end() const noexcept
        {
            return elements_.end();
        }

        [[nodiscard]] constexpr auto closest_to(const Coordinate& where) const noexcept
            -> std::optional<ClosestItem<const T&, details::ElementType<Coordinate>>>
        {
            if (size() == 0) [[unlikely]]
                return std::nullopt;

            std::optional<details::ClosestItem<Coordinate>> closest_item{};

            _root().find_closest(where, closest_item);

            if (!closest_item.has_value()) [[unlikely]]
                return std::nullopt;

            const auto [index, distance] = closest_item.value();

            return ClosestItem<const T&, details::ElementType<Coordinate>>{elements_[index], distance};
        }

        void debug_print() const noexcept
        {
            _root().debug_print(0U);
        }

        [[nodiscard]] constexpr const auto& elements() const noexcept
        {
            return elements_;
        }

        constexpr ~OcTree() noexcept = default;

    private:
        [[nodiscard]] constexpr Node& _root() noexcept
        {
            return root_;
        }

        [[nodiscard]] constexpr const Node& _root() const noexcept
        {
            return root_;
        }

        constexpr void _build_from_items() noexcept
        {
            if (elements_.empty())
                return;

            for (std::size_t i{}; i != elements_.size(); ++i) {
                _root().insert(i, _get_bounding_box(elements_[i]));
            }
        }

        Node root_{};
        std::vector<T> elements_{};
        GetBoundingBox _get_bounding_box{};
    };

} //namespace Raychel

#endif //!RAYCHELCORE_OCTTREE_H
