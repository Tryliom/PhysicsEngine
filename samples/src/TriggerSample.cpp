#include "TriggerSample.h"

#include "Random.h"
#include "Display.h"

void TriggerSample::onInit() noexcept
{
    Display::SetTitle("Trigger Sample");
    _world.SetContactListener(this);

	constexpr static int TriggerObjectCount = 10;

	_objects.resize(TriggerObjectCount * 3);

	for (int i = 0; i < TriggerObjectCount; ++i)
	{
        createBall();
        createBox();
        createPolygon();
	}
}

void TriggerSample::onDeinit() noexcept
{
    _objects.clear();
}

void TriggerSample::onUpdate(float deltaTime) noexcept
{
    const auto screenWidth = static_cast<float>(Display::GetWidth());
    const auto screenHeight = static_cast<float>(Display::GetHeight());

    // Check if the object is outside the screen, and if so, teleport it to the other side
    // Set up also his color to normal
    for (auto& object : _objects)
    {
        auto& body = _world.GetBody(object.BodyRef);

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

        if (object.TriggerEnterTimer > 0.f)
        {
            object.TriggerEnterTimer -= deltaTime;
        }
        else
        {
            object.TriggerEnterTimer = 0.f;
        }

        if (object.TriggerExitTimer > 0.f)
        {
            object.TriggerExitTimer -= deltaTime;
        }
        else
        {
            object.TriggerExitTimer = 0.f;
        }
    }
}

void TriggerSample::onRender() noexcept
{
    for (auto& object : _objects)
    {
        const auto& body = _world.GetBody(object.BodyRef);
        const auto& collider = _world.GetCollider(object.ColliderRef);

        switch(collider.GetShapeType())
        {
            case Math::ShapeType::Circle:
            {
                const auto circle = collider.GetCircle() + body.Position();

                if (object.TriggerEnterTimer > 0.f)
                {
                    Display::Draw({circle.Center(), circle.Radius() * 1.4f}, _triggerEnterColor);
                }

                if (object.TriggerExitTimer > 0.f)
                {
                    Display::Draw({circle.Center(), circle.Radius() * 1.2f}, _triggerExitColor);
                }

                Display::Draw(circle, object.Color);
            }
                break;

            case Math::ShapeType::Rectangle:
            {
                const auto rect = collider.GetRectangle() + body.Position();

                if (object.TriggerEnterTimer > 0.f)
                {
                    Display::Draw(rect, _triggerEnterColor, Math::Vec2F(1.4f, 1.4f));
                }

                if (object.TriggerExitTimer > 0.f)
                {
                    Display::Draw(rect, _triggerExitColor, Math::Vec2F(1.2f, 1.2f));
                }

                Display::Draw(rect, object.Color);
            }
                break;

            case Math::ShapeType::Polygon:
            {
                auto poly = collider.GetPolygon() + body.Position();

                if (object.TriggerEnterTimer > 0.f)
                {
                    Display::Draw(poly, _triggerEnterColor, Math::Vec2F(1.4f, 1.4f));
                }

                if (object.TriggerExitTimer > 0.f)
                {
                    Display::Draw(poly, _triggerExitColor, Math::Vec2F(1.2f, 1.2f));
                }

                Display::Draw(poly, object.Color);
            }
            case Math::ShapeType::None: break;
        }
    }

    // Clear object color
    for (auto& object : _objects)
    {
        object.Color = _color;
    }
}

void TriggerSample::createBall() noexcept
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const float minRadius = 10.f;
	const float maxRadius = 30.f;

	auto circle = Math::CircleF{ {0.f, 0.f}, Math::Random::Range(minRadius, maxRadius) };

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetCircle(circle);
	collider.SetIsTrigger(true);
}

void TriggerSample::createBox() noexcept
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

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetRectangle(rect);
	collider.SetIsTrigger(true);
}

void TriggerSample::createPolygon() noexcept
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

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetPolygon(poly);
	collider.SetIsTrigger(true);
}

void TriggerSample::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto & _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.TriggerEnterTimer = _blinkTimer;
        }
    }
}

void TriggerSample::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto & _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.TriggerExitTimer = _blinkTimer;
        }
    }
}

void TriggerSample::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto & _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.Color = _triggerStayColor;
        }
    }
}
