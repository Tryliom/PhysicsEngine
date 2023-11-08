#include "QuadTree.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace Physics
{
	QuadNode::QuadNode(HeapAllocator& allocator) noexcept :
		Colliders {StandardAllocator<SimplifiedCollider> {allocator}} {}

	QuadTree::QuadTree(const Math::RectangleF& boundary) noexcept :
		_nodesAllocator(std::malloc((getMaxNodes()) * sizeof(QuadNode) * 2), (getMaxNodes()) * sizeof(QuadNode) * 2)
    {
		_nodes.resize(getMaxNodes(), QuadNode {_heapAllocator});

        UpdateBoundary(boundary);
    }

    constexpr std::size_t QuadTree::getMaxNodes() noexcept
    {
        std::size_t nodes = 0;

        for (auto i = 0; i <= _maxDepth; i++)
        {
            nodes += static_cast<std::size_t>(Math::Pow(4, i));
        }

        return nodes;
    }

    constexpr std::size_t QuadTree::getDepth(std::size_t index) noexcept
    {
        std::size_t depth = 0;

        while (index > 0)
        {
            index /= 4;
            depth++;
        }

        return depth;
    }

    void QuadTree::subdivide(std::size_t index) noexcept
    {
#ifdef TRACY_ENABLE
	    ZoneNamedN(subdivide, "QuadTree::subdivide", true);
#endif

        auto& node = _nodes[index];

        if (node.Divided) return;

        _nodes[index].Divided = true;

        // Distribute colliders to children, when a collider is distributed, remove it from the parent. If it collides with more than one child, keep it in the parent.
        const auto colliders = node.Colliders;

        node.Colliders.clear();

        for (auto collider : colliders)
        {
            std::size_t targetIndex = 0;

            for (auto i = 1; i <= 4; i++)
            {
                const auto& child = _nodes[index * 4 + i];

                if (Math::Intersect(child.Boundary, collider.Bounds))
                {
                    if (targetIndex != 0)
                    {
                        targetIndex = index;
                        break;
                    }

                    targetIndex = index * 4 + i;
                }
            }

            _nodes[targetIndex].Colliders.push_back(collider);
        }
    }

	void QuadTree::Insert(SimplifiedCollider collider) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(insert, "QuadTree::Insert", true);
#endif

        std::size_t parentIndex = 0;

        while (true)
        {
            auto& node = _nodes[parentIndex];

            if (node.Divided)
            {
                // Check if it collides with more than one child
                // True -> Push it in the parent
                // False -> If parentIndex is at max depth, put it in, if not, set parentIndex to it
                std::size_t targetIndex = 0;

                for (auto i = 1; i <= 4; i++)
                {
                    const auto& child = _nodes[parentIndex * 4 + i];

                    if (Math::Intersect(child.Boundary, collider.Bounds))
                    {
                        if (targetIndex != 0)
                        {
                            targetIndex = parentIndex;
                            break;
                        }

                        targetIndex = parentIndex * 4 + i;
                    }
                }

                if (targetIndex != parentIndex)
                {
                    parentIndex = targetIndex;
                }
                else
                {
                    _nodes[targetIndex].Colliders.push_back(collider);
                    break;
                }
            }
            else
            {
                node.Colliders.push_back(collider);

                if (node.Colliders.size() > _maxCapacity && getDepth(parentIndex) < _maxDepth)
                {
                    subdivide(parentIndex);
                }

                break;
            }
        }
	}

	void QuadTree::addAllPossiblePairs(std::size_t index, ColliderRef collider) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif

		const auto& node = _nodes[index];

		for (auto i = 0; i < node.Colliders.size(); i++)
		{
			const auto& otherCollider = node.Colliders[i];

			if (collider == otherCollider.Ref) continue;

			if (Math::Intersect(otherCollider.Bounds, _nodes[index].Colliders[i].Bounds))
			{
				_allPossiblePairs.push_back(ColliderPair{collider, otherCollider.Ref});
			}
		}

		if (node.Divided)
		{
			for (auto i = 1; i <= 4; i++)
			{
				addAllPossiblePairs(index * 4 + i, collider);
			}
		}
	}

	const MyVector<ColliderPair>& QuadTree::GetAllPossiblePairs() noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(GetAllPossiblePairs, "QuadTree::GetAllPossiblePairs", true);
#endif
		for (std::size_t parentIndex = 0; parentIndex < _nodes.size(); parentIndex++)
		{
			const auto& node = _nodes[parentIndex];

			for (auto i = 0; i < node.Colliders.size(); i++)
			{
				const auto& collider = node.Colliders[i];

				for (auto j = i + 1; j < node.Colliders.size(); j++)
				{
					const auto& otherCollider = node.Colliders[j];

					if (collider.Ref == otherCollider.Ref) continue;

					if (Math::Intersect(collider.Bounds, otherCollider.Bounds))
					{
						_allPossiblePairs.push_back(ColliderPair{collider.Ref, otherCollider.Ref});
					}
				}

				if (node.Divided)
				{
					for (auto j = 1; j <= 4; j++)
					{
						addAllPossiblePairs(parentIndex * 4 + j, collider.Ref);
					}
				}
			}
		}

		return _allPossiblePairs;
	}

	void QuadTree::UpdateBoundary(const Math::RectangleF& boundary) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(updateBoundary, "QuadTree::UpdateBoundary", true);
#endif

        _nodes[0].Boundary = boundary;
        std::size_t index = 1;

        for (std::size_t i = 0; index < getMaxNodes(); i++)
        {
            const auto bounds = _nodes[i].Boundary;
            const auto minBound = bounds.MinBound();
            const auto halfSize = bounds.Size() / 2.f;

            _nodes[index].Boundary = Math::RectangleF(minBound, bounds.Center());
            _nodes[index+1].Boundary = Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y),
                                                    Math::Vec2F(minBound.X + halfSize.X, minBound.Y) + halfSize);
            _nodes[index+2].Boundary = Math::RectangleF(Math::Vec2F(minBound.X, minBound.Y + halfSize.Y),
                                                      Math::Vec2F(minBound.X, minBound.Y + halfSize.Y) + halfSize);
            _nodes[index+3].Boundary = Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y),
                                                      Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y) + halfSize);

            index += 4;
        }
	}

	void QuadTree::ClearColliders() noexcept
	{
		for (auto& node : _nodes)
        {
            node.Colliders.clear();
            node.Divided = false;
        }

		_allPossiblePairs.clear();
	}

	std::vector<Math::RectangleF> QuadTree::GetBoundaries() const noexcept
	{
		std::vector<Math::RectangleF> boundaries;

		boundaries.reserve(_nodes.size());

        for (auto& node : _nodes)
        {
            if (node.Colliders.empty()) continue;

            boundaries.push_back(node.Boundary);
        }

		return boundaries;
	}

	std::size_t QuadTree::GetAllCollidersCount() const noexcept
	{
		std::size_t count = 0;

		for (auto& node : _nodes)
        {
            count += node.Colliders.size();
        }

		return count;
	}
}