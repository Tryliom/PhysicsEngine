#pragma once

#include "Collider.h"

#include <vector>
#include <memory>

namespace Physics
{
	class QuadTree
	{
	public:
		QuadTree() noexcept = default;
		explicit QuadTree(const Math::RectangleF& boundary) noexcept;
		~QuadTree() noexcept;

	private:
		std::vector<const Collider*> _colliders;
		std::array<QuadTree*, 4> _nodes { nullptr, nullptr, nullptr, nullptr };
		Math::RectangleF _boundary {Math::Vec2F::Zero(), Math::Vec2F::One()};

		static constexpr std::size_t _maxDepth = 4;
		static constexpr std::size_t _maxCapacity = 16;

	public:
		/**
		 * @brief Insert a collider into the quadtree, if the quadtree is full, subdivide the quadtree and insert the collider into the correct node
		 * @param collider The collider to insert
		 * @param depth The current depth of the quadtree
		 */
		void Insert(const Collider* collider, int depth = 0) noexcept;
		/**
		 * @brief Get all colliders that overlap with the collider
		 * @param collider The collider to check for overlap
		 * @return All colliders that overlap with the collider
		 */
		std::vector<const Collider*> GetColliders(const Collider* collider) const noexcept;

		/**
		 * @brief Preallocate quadtree nodes with 4^MaxDepth nodes
		 * @param depth The current depth of the quadtree
		 */
		void Preallocate(int depth = 0) noexcept;
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
		 * @brief Get all the boundaries of the quadtree nodes
		 * @return All the boundaries of the quadtree nodes
		 */
		[[nodiscard]] std::vector<Math::RectangleF> GetBoundaries() const noexcept;

		static constexpr std::size_t MaxDepth() noexcept { return _maxDepth; }
		static constexpr std::size_t MaxCapacity() noexcept { return _maxCapacity; }

	private:
		/**
		 * @brief Subdivide the quadtree
		 * @param update If true, the boundary of the nodes will be set to the correct position without creating new nodes
		 */
		void subdivide(bool update) noexcept;
	};
}