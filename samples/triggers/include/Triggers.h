#pragma once

#include "TriggerListener.h"
#include "World.h"
#include "Trigger.h"

struct Object
{
	Physics::BodyRef BodyRef{};
	Physics::ColliderRef ColliderRef{};
	std::size_t TriggerIndex{};
	Physics::Shape Shape;
	Math::ShapeType ShapeType = Math::ShapeType::Circle;
};

class Triggers
{
public:
	Triggers();

private:
	Physics::World _world;
	std::vector<Trigger> _triggers;
	std::vector<Object> _objects;

	const Color _color = Color(100, 100, 100);
	const Color _triggerEnterColor = Color(100, 255, 100);
	const Color _triggerExitColor = Color(255, 100, 100);

	void CreateBall();
	void CreateBox();
	void CreatePolygon();

public:
	void Update(float deltaTime) noexcept;
	void Render() noexcept;
};