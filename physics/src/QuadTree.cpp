#include "QuadTree.h"

namespace Physics
{
	QuadTree::QuadTree(const Math::RectangleF& boundary) noexcept : _boundary(boundary) {}

	QuadTree::~QuadTree() noexcept
	{
		for (auto* node : _nodes)
		{
			if (node == nullptr) continue;

			delete node;
		}
	}

	void QuadTree::Insert(const Collider* collider, int depth) noexcept
	{
		if (!Math::Intersect(_boundary, collider->GetBounds())) return;

		if (_colliders.size() < _maxCapacity || depth >= _maxDepth)
		{
			_colliders.push_back(collider);
			return;
		}

		for (auto& node : _nodes)
		{
			node->Insert(collider, depth + 1);
		}
	}

	std::vector<const Collider*> QuadTree::GetColliders(const Collider* collider) const noexcept
	{
		std::vector<const Collider*> colliders;

		// Need to have body position for GetBounds
		if (!Math::Intersect(_boundary, collider->GetBounds())) return colliders;

		for (auto& node : _nodes)
		{
			if (node == nullptr) continue;

			auto nodeColliders = node->GetColliders(collider);

			colliders.insert(colliders.end(), nodeColliders.begin(), nodeColliders.end());
		}

		colliders.insert(colliders.end(), _colliders.begin(), _colliders.end());

		return colliders;
	}

	void QuadTree::Preallocate(int depth) noexcept
	{
		if (depth >= _maxDepth) return;

		if (_nodes[0] == nullptr)
		{
			subdivide(false);
		}

		for (auto& node : _nodes)
		{
			node->Preallocate(depth + 1);
		}
	}

	void QuadTree::UpdateBoundary(const Math::RectangleF& boundary) noexcept
	{
		_boundary = boundary;

		subdivide(true);
	}

	void QuadTree::ClearColliders() noexcept
	{
		_colliders.clear();

		for (auto& node : _nodes)
		{
			if (node == nullptr) continue;

			node->ClearColliders();
		}
	}

	std::vector<Math::RectangleF> QuadTree::GetBoundaries() const noexcept
	{
		std::vector<Math::RectangleF> boundaries;

		boundaries.push_back(_boundary);

		for (auto& node : _nodes)
		{
			if (node == nullptr) continue;

			auto nodeBoundaries = node->GetBoundaries();
			boundaries.insert(boundaries.end(), nodeBoundaries.begin(), nodeBoundaries.end());
		}

		return boundaries;
	}

	void QuadTree::subdivide(bool update) noexcept
	{
		const auto minBound = _boundary.MinBound();
		const auto halfSize = _boundary.Size() / 2.f;

		if (update)
		{
			if (_nodes[0] == nullptr) return;

			_nodes[0]->UpdateBoundary(Math::RectangleF(minBound, _boundary.Center()));
			_nodes[1]->UpdateBoundary(Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y), Math::Vec2F(minBound.X + halfSize.X, minBound.Y) + halfSize));
			_nodes[2]->UpdateBoundary(Math::RectangleF(Math::Vec2F(minBound.X, minBound.Y + halfSize.Y), Math::Vec2F(minBound.X, minBound.Y + halfSize.Y) + halfSize));
			_nodes[3]->UpdateBoundary(Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y), Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y) + halfSize));
		}
		else
		{
			_nodes[0] = new QuadTree(Math::RectangleF(minBound, _boundary.Center()));
			_nodes[1] = new QuadTree(Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y), Math::Vec2F(minBound.X + halfSize.X, minBound.Y) + halfSize));
			_nodes[2] = new QuadTree(Math::RectangleF(Math::Vec2F(minBound.X, minBound.Y + halfSize.Y), Math::Vec2F(minBound.X, minBound.Y + halfSize.Y) + halfSize));
			_nodes[3] = new QuadTree(Math::RectangleF(Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y), Math::Vec2F(minBound.X + halfSize.X, minBound.Y + halfSize.Y) + halfSize));
		}
	}
}