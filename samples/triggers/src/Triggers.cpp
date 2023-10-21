#include "Triggers.h"

#include "Random.h"

Triggers::Triggers()
{
	constexpr static int TriggerObjectCount = 10;

	_objects.resize(TriggerObjectCount * 3);
	_triggers.reserve(TriggerObjectCount * 3);

	for (int i = 0; i < TriggerObjectCount; ++i)
	{
		CreateBall();
		CreateBox();
		CreatePolygon();
	}
}

void Triggers::CreateBall()
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const float minRadius = 10.f;
	const float maxRadius = 30.f;

	auto circle = Math::CircleF{ {0.f, 0.f}, Math::Random::Range(minRadius, maxRadius) };

	_triggers.emplace_back();

	_objects.emplace_back();
	_objects.back().ShapeType = Math::ShapeType::Circle;
	_objects.back().Shape.Circle = circle;
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);
	_objects.back().TriggerIndex = _triggers.size() - 1;

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetCircle(circle);
	collider.SetIsTrigger(true);
	collider.SetContactListener(new TriggerListener(_triggers, _triggers.size() - 1));
}

void Triggers::CreateBox()
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const float minBoundMin = 10.f;
	const float minBoundMax = 50.f;
	const float maxBoundMin = 60.f;
	const float maxBoundMax = 100.f;

	auto rect = Math::RectangleF{
		{Math::Random::Range(minBoundMin, minBoundMax), Math::Random::Range(minBoundMin, minBoundMax)},
		{Math::Random::Range(maxBoundMin, maxBoundMax), Math::Random::Range(maxBoundMin, maxBoundMax)}
	};

	_triggers.emplace_back();

	_objects.emplace_back();
	_objects.back().ShapeType = Math::ShapeType::Rectangle;
	_objects.back().Shape.Rectangle = rect;
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);
	_objects.back().TriggerIndex = _triggers.size() - 1;

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetRectangle(rect);
	collider.SetIsTrigger(true);
	collider.SetContactListener(new TriggerListener(_triggers, _triggers.size() - 1));
}

void Triggers::CreatePolygon()
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const int randomVertices = Math::Random::Range(3, 8);
	const float min = 10.f;
	const float max = 70.f;

	std::vector<Math::Vec2F> vertices;

	vertices.resize(randomVertices);

	for (int i = 0; i < randomVertices; ++i)
	{
		vertices.emplace_back(Math::Random::Range(min, max), Math::Random::Range(min, max));
	}

	auto poly = Math::PolygonF{ vertices };

	_triggers.emplace_back();

	_objects.emplace_back();
	_objects.back().ShapeType = Math::ShapeType::Polygon;
	_objects.back().Shape.Polygon = poly;
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);
	_objects.back().TriggerIndex = _triggers.size() - 1;

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetPolygon(poly);
	collider.SetIsTrigger(true);
	collider.SetContactListener(new TriggerListener(_triggers, _triggers.size() - 1));
}

void Triggers::Update(float deltaTime) noexcept
{
	const auto screenWidth = static_cast<float>(Display::GetWidth());
	const auto screenHeight = static_cast<float>(Display::GetHeight());

	// Check if the object is outside the screen, and if so, teleport it to the other side
	// Set up also his color to normal
	for (auto& object : _objects)
	{
		auto& body = _world.GetBody(object.BodyRef);
		auto& trigger = _triggers[object.TriggerIndex];

		trigger.Color = _color;

		if (body.Position().X < 0.f)
		{
			body.SetPosition({ screenWidth, body.Position().Y });
		}
		else if (body.Position().X > screenWidth)
		{
			body.SetPosition({ 0.f, body.Position().Y });
		}

		if (body.Position().Y < 0.f)
		{
			body.SetPosition({ body.Position().X, screenHeight });
		}
		else if (body.Position().Y > screenHeight)
		{
			body.SetPosition({ body.Position().X, 0.f });
		}

		if (trigger.TriggerEnterTimer > 0.f)
		{
			trigger.TriggerEnterTimer -= deltaTime;
		}
		else
		{
			trigger.TriggerEnterTimer = 0.f;
		}

		if (trigger.TriggerExitTimer > 0.f)
		{
			trigger.TriggerExitTimer -= deltaTime;
		}
		else
		{
			trigger.TriggerExitTimer = 0.f;
		}
	}

	_world.Update(deltaTime);
}

void Triggers::Render() noexcept
{
	for (auto& object : _objects)
	{
		const auto& body = _world.GetBody(object.BodyRef);
		const auto& trigger = _triggers[object.TriggerIndex];

		if (object.ShapeType == Math::ShapeType::Circle)
		{
			const auto circle = object.Shape.Circle + body.Position();

			if (trigger.TriggerEnterTimer > 0.f)
			{
				Display::Draw({circle.Center(), circle.Radius() * 1.4f}, _triggerEnterColor);
			}

			if (trigger.TriggerExitTimer > 0.f)
			{
				Display::Draw({circle.Center(), circle.Radius() * 1.2f}, _triggerExitColor);
			}

			Display::Draw(circle, trigger.Color);
		}
		else if (object.ShapeType == Math::ShapeType::Rectangle)
		{
			const auto rect = object.Shape.Rectangle + body.Position();

			if (trigger.TriggerEnterTimer > 0.f)
			{
				Display::Draw(rect, _triggerEnterColor, Math::Vec2F(1.4f, 1.4f));
			}

			if (trigger.TriggerExitTimer > 0.f)
			{
				Display::Draw(rect, _triggerExitColor, Math::Vec2F(1.2f, 1.2f));
			}

			Display::Draw(rect, trigger.Color);
		}
		else if (object.ShapeType == Math::ShapeType::Polygon)
		{
			auto poly = object.Shape.Polygon + body.Position();

			if (trigger.TriggerEnterTimer > 0.f)
			{
				Display::Draw(poly, _triggerEnterColor, Math::Vec2F(1.4f, 1.4f));
			}

			if (trigger.TriggerExitTimer > 0.f)
			{
				Display::Draw(poly, _triggerExitColor, Math::Vec2F(1.2f, 1.2f));
			}

			Display::Draw(poly, trigger.Color);
		}
	}
}