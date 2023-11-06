#pragma once

#include "Collider.h"
#include "UniquePtr.h"
#include "Allocator.h"

#include <vector>
#include <memory>

namespace Physics
{
    struct SimplifiedCollider
    {
        ColliderRef Ref {};
        Math::RectangleF Bounds {Math::Vec2F::Zero(), Math::Vec2F::One()};
    };

    struct QuadNode
    {
		explicit QuadNode(HeapAllocator& allocator) noexcept;

	    MyVector<SimplifiedCollider> Colliders;
        Math::RectangleF Boundary {Math::Vec2F::Zero(), Math::Vec2F::One()};
        bool Divided {false};
    };

	class QuadTree
	{
	public:
        /**
         * @brief Preallocate quadtree nodes with 4^MaxDepth nodes
         * @param boundary The boundary of the quadtree
         */
		explicit QuadTree(const Math::RectangleF& boundary) noexcept;

	private:
		HeapAllocator _heapAllocator {};
		LinearAllocator _nodesAllocator;
		FreeListAllocator _colliderAllocator;
		MyVector<QuadNode> _nodes { StandardAllocator <QuadNode> {_nodesAllocator} };

        static constexpr std::size_t _maxDepth = 5;
		static constexpr std::size_t _maxCapacity = 8;

        static constexpr std::size_t getMaxNodes() noexcept;
        static constexpr std::size_t getDepth(std::size_t index) noexcept;

        void subdivide(std::size_t index) noexcept;
        [[nodiscard]] MyVector<ColliderRef> getColliders(std::size_t index, SimplifiedCollider collider) noexcept;

    public:
		/**
		 * @brief Insert a collider into the quadtree, if the quadtree is full, subdivide the quadtree and insert the collider into the correct node
		 * @param collider The collider to insert
		 * @param depth The current depth of the quadtree
		 */
		void Insert(SimplifiedCollider collider) noexcept;
		/**
		 * @brief Get all colliders that overlap with the collider
		 * @param collider The collider to check for overlap
		 * @return All colliders that overlap with the collider
		 */
		[[nodiscard]] MyVector<ColliderRef> GetColliders(SimplifiedCollider collider) noexcept;

		/**
		 * @brief Set the new boundary of the quadtree, applies to all nodes
		 * @param boundary The new boundary
		 */
		void UpdateBoundary(const Math::RectangleF& boundary) noexcept;

		/**
		 * @brief Clear all colliders from the quadtree
		 */
		void ClearColliders() noexcept;

		/**
		 * @brief Get all the boundaries of the quadtree nodes that are not nullptr or empty
		 * @return All the boundaries of the quadtree nodes
		 */
		[[nodiscard]] std::vector<Math::RectangleF> GetBoundaries() const noexcept;
		/**
		 * @brief Get the number of colliders in the quadtree and all its nodes
		 * @return The number of colliders in the quadtree and all its nodes
		 */
		[[nodiscard]] std::size_t GetAllCollidersCount() const noexcept;

		static constexpr std::size_t MaxDepth() noexcept { return _maxDepth; }
		static constexpr std::size_t MaxCapacity() noexcept { return _maxCapacity; }
	};
}