#pragma once

#include "Body.h"
#include "Collider.h"
#include "ColliderPair.h"
#include "ContactListener.h"
#include "QuadTree.h"
#include "Allocator.h"

#include <vector>
#include <unordered_set>

namespace Physics
{
    /**
     * @brief The world class is the main class of the physics engine. It contains all bodies and colliders.
     */
    class World
    {
    public:
        explicit World(std::size_t defaultBodySize = 500) noexcept;

    private:
		std::unordered_set<ColliderPair, ColliderPairHash> _colliderPairs;

		std::vector<Collider> _colliders;
	    std::vector<std::size_t> _colliderGenerations;
        std::vector<Body> _bodies;
        std::vector<std::size_t> _bodyGenerations;
		QuadTree _quadTree {};

        ContactListener* _contactListener { nullptr };

		/**
		 * @brief Check the collisions and triggers of the colliders
		 */
		void updateColliders() noexcept;
		/**
		 * @brief Check if the colliders overlap
		 * @param colliderA	 The first collider
		 * @param colliderB  The second collider
		 * @return True if the colliders overlap
		 */
		[[nodiscard]] static bool overlap(const Collider& colliderA, const Collider& colliderB) noexcept;

    public:
		/**
		 * @brief Update the world
		 * @param deltaTime The time since the last update
		 */
        void Update(float deltaTime) noexcept;

		/**
		 * @brief Create a body. Sets the bodyRef of the body. Enables the body
		 * @return The bodyRef of the created body
		 */
        BodyRef CreateBody() noexcept;
		/**
		 * @brief Destroy a body and all its colliders
		 * @param bodyRef The body to destroy
		 */
        void DestroyBody(BodyRef bodyRef);
		/**
		 * @brief Get a body
		 * @param bodyRef The bodyRef of the body
		 * @return The body
		 */
        Body& GetBody(BodyRef bodyRef);

		/**
		 * @brief Create a collider for a body. Sets the bodyRef and colliderRef of the collider. Enables the collider.
		 * @param bodyRef The body to create the collider for.
		 * @return The colliderRef of the created collider.
		 */
		ColliderRef CreateCollider(BodyRef bodyRef) noexcept;
		/**
		 * @brief Destroy a collider
		 * @param colliderRef The collider to destroy
		 */
		void DestroyCollider(ColliderRef colliderRef);
		/**
		 * @brief Get a collider
		 * @param colliderRef The colliderRef of the collider
		 * @return The collider
		 */
		Collider& GetCollider(ColliderRef colliderRef);

		/**
		 * @brief Set the contact listener of the world for collision and trigger events, there is only one callback for both events
		 * @param contactListener The contact listener
		 */
        void SetContactListener(ContactListener* contactListener) noexcept;

	    /**
		 * @brief Get all the boundaries of the quadtree
		 * @return All the boundaries of the quadtree
		 */
	    [[nodiscard]] std::vector<Math::RectangleF> GetQuadTreeBoundaries() const noexcept;
    };
}