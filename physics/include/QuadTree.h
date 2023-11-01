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
        //TODO: Use uniquePtr and make it a vector with all the nodes instead of recursive
		std::array<QuadTree*, 4> _nodes { nullptr, nullptr, nullptr, nullptr };
		Math::RectangleF _boundary {Math::Vec2F::Zero(), Math::Vec2F::One()};

		static constexpr std::size_t _maxDepth = 4;
		static constexpr std::size_t _maxCapacity = 8;

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
		 * @brief Get all the boundaries of the quadtree nodes that are not nullptr or empty
		 * @return All the boundaries of the quadtree nodes
		 */
		[[nodiscard]] std::vector<Math::RectangleF> GetBoundaries() const noexcept;
		/**
		 * @brief Get the number of colliders in the quadtree
		 * @return The number of colliders in the quadtree
		 */
		[[nodiscard]] std::size_t GetCollidersCount() const noexcept { return _colliders.size(); }
		/**
		 * @brief Get the number of colliders in the quadtree and all its nodes
		 * @return The number of colliders in the quadtree and all its nodes
		 */
		[[nodiscard]] std::size_t GetAllCollidersCount() const noexcept;

		static constexpr std::size_t MaxDepth() noexcept { return _maxDepth; }
		static constexpr std::size_t MaxCapacity() noexcept { return _maxCapacity; }

	private:
		/**
		 * @brief Subdivide the quadtree, create them if nullptr, apply the new boundaries to the nodes
		 * @param update If true, apply the new boundaries to the nodes without creating them
		 */
		void subdivide(bool update) noexcept;
	};
}