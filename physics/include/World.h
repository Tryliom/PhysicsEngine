#pragma once

#include "Body.h"
#include "Collider.h"
#include "ColliderPair.h"
#include "ContactListener.h"

#include <vector>
#include <unordered_set>

namespace Physics
{
    class World
    {
    public:
        explicit World(std::size_t defaultBodySize = 500) noexcept;

    private:
        std::vector<Body> _bodies;
        std::vector<std::size_t> _bodyGenerations;
		std::vector<Collider> _colliders;
	    std::vector<std::size_t> _colliderGenerations;
		std::unordered_set<ColliderPair, ColliderPairHash> _colliderPairs;

        ContactListener* _contactListener { nullptr };

		void updateColliders() noexcept;
		[[nodiscard]] bool overlap(Collider& colliderA, Collider& colliderB) noexcept;

    public:
        void Update(float deltaTime) noexcept;

        BodyRef CreateBody() noexcept;
        void DestroyBody(BodyRef bodyRef);
        Body& GetBody(BodyRef bodyRef);

		/**
		 * @brief Create a collider for a body. Sets the bodyRef and colliderRef of the collider. Enables the collider.
		 * @param bodyRef The body to create the collider for.
		 * @return The colliderRef of the created collider.
		 */
		ColliderRef CreateCollider(BodyRef bodyRef) noexcept;
		void DestroyCollider(ColliderRef colliderRef);
		Collider& GetCollider(ColliderRef colliderRef);

        void SetContactListener(ContactListener* contactListener) noexcept;
    };
}