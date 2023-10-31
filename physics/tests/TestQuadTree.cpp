#include "QuadTree.h"

#include "gtest/gtest.h"

struct TestQuadTreeFixture : public ::testing::TestWithParam<Math::RectangleF> {};

INSTANTIATE_TEST_SUITE_P(QuadTree, TestQuadTreeFixture, testing::Values(
	Math::RectangleF(Math::Vec2F(0, 0), Math::Vec2F(1, 1)),
	Math::RectangleF(Math::Vec2F(0.3f, 1.3f), Math::Vec2F(0.634f, 2.f)),
	Math::RectangleF(Math::Vec2F(0.f, 0.f), Math::Vec2F(300.f, 300.f)),
	Math::RectangleF(Math::Vec2F(-500.f, -700.f), Math::Vec2F(1'000.f, 1'200.f))
));

TEST_P(TestQuadTreeFixture, DefaultConstructor)
{
	Physics::QuadTree quadTree(GetParam());

	quadTree.Preallocate();

	auto boundaries = quadTree.GetBoundaries();

	EXPECT_EQ(boundaries.size(), 1);
}

TEST_P(TestQuadTreeFixture, InsertTopLeft)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;

	quadTree.Preallocate();

	std::vector<Physics::Collider> colliders;

	for (auto i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.emplace_back();
		colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
		colliders.back().SetPosition(rect.MinBound() + collidersSize * 2.f);
		quadTree.Insert(&colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());

	colliders.emplace_back();
	colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
	colliders.back().SetPosition(rect.MinBound() + collidersSize * 2.f);
	quadTree.Insert(&colliders.back());

	EXPECT_EQ(quadTree.GetBoundaries().size(), 2);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());
	EXPECT_EQ(quadTree.GetAllCollidersCount(), Physics::QuadTree::MaxCapacity() + 1);
}

TEST_P(TestQuadTreeFixture, InsertMid)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;

	quadTree.Preallocate();

	std::vector<Physics::Collider> colliders;

	for (auto i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.emplace_back();
		colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
		colliders.back().SetPosition(rect.Center());
		quadTree.Insert(&colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());

	colliders.emplace_back();
	colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
	colliders.back().SetPosition(rect.Center());
	quadTree.Insert(&colliders.back());

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1 + 4);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());
	EXPECT_EQ(quadTree.GetAllCollidersCount(), Physics::QuadTree::MaxCapacity() + 4);
}

TEST_P(TestQuadTreeFixture, GetColliders)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;

	quadTree.Preallocate();

	std::vector<Physics::Collider> colliders;

	for (auto i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.emplace_back();
		colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
		colliders.back().SetPosition(rect.Center());
		quadTree.Insert(&colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());

	colliders.emplace_back();
	colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
	colliders.back().SetPosition(rect.Center());
	quadTree.Insert(&colliders.back());

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1 + 4);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());
	EXPECT_EQ(quadTree.GetAllCollidersCount(), Physics::QuadTree::MaxCapacity() + 4);

	auto colliders2 = quadTree.GetColliders(&colliders.back());

	EXPECT_EQ(colliders2.size(), Physics::QuadTree::MaxCapacity() + 4);
}

TEST_P(TestQuadTreeFixture, ClearColliders)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;

	quadTree.Preallocate();

	std::vector<Physics::Collider> colliders;

	for (auto i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.emplace_back();
		colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
		colliders.back().SetPosition(rect.Center());
		quadTree.Insert(&colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());

	colliders.emplace_back();
	colliders.back().SetRectangle(Math::RectangleF(-collidersSize / 2.f, collidersSize / 2.f));
	colliders.back().SetPosition(rect.Center());
	quadTree.Insert(&colliders.back());

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1 + 4);
	EXPECT_EQ(quadTree.GetCollidersCount(), Physics::QuadTree::MaxCapacity());
	EXPECT_EQ(quadTree.GetAllCollidersCount(), Physics::QuadTree::MaxCapacity() + 4);

	quadTree.ClearColliders();

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetCollidersCount(), 0);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), 0);
}