#include "CollisionSample.h"

#include "Random.h"
#include "Display.h"
#include "Input.h"

CollisionSample::CollisionSample() noexcept :
	Sample(
		"Collisions",
		"Sample showing how to use colliders with circles and boxes.\n"
		"Controls:\n"
		"Space -> Freeze/unfreeze all objects\n"
		"B -> Toggle boxes around objects\n"
		"Q -> Toggle quad trees\n"
		"Right click -> Move camera\n"
		"Mouse wheel -> Zoom\n"
	)
{}

void CollisionSample::onInit() noexcept
{
    Display::SetTitle("Trigger Sample");
    _world.SetContactListener(this);

	constexpr static int Circles = 0;
    constexpr static int Boxes = 30;

	_objects.resize(Circles + Boxes + 4 * _wallSplit);

	for (int i = 0; i < Circles; ++i)
	{
        createBall();
	}

    for (int i = 0; i < Boxes; ++i)
    {
        createBox();
    }

	// Create walls
	createWalls();
}

void CollisionSample::onDeinit() noexcept
{
    _objects.clear();
}

void CollisionSample::onInput() noexcept
{
	const auto screenWidth = static_cast<float>(Display::GetWidth());
	const auto screenHeight = static_cast<float>(Display::GetHeight());
	const auto mouseWheelDelta = Input::GetMouseWheelDelta();

	if (mouseWheelDelta != 0)
	{
		Math::Vec2F target = Display::GetMousePosition();

		Display::SetCameraZoom(Display::GetCameraZoom() + mouseWheelDelta * 0.05f, target);
	}

	auto mouseDelta = Display::GetMouseDelta();

	// Move camera when holding right mouse button.
	if (Input::IsMouseButtonHeld(SDL_BUTTON_RIGHT) && mouseDelta != Math::Vec2F::Zero())
	{
		Display::MoveCamera(mouseDelta);
	}

	// Toggle freeze/unfreeze of all objects by pressing space.
	if (Input::IsKeyPressed(SDL_SCANCODE_SPACE))
	{
		_stop = !_stop;

		for (auto& object : _objects)
		{
			auto& body = _world.GetBody(object.BodyRef);
			auto velocity = Math::Vec2F::Zero();

			if (!_stop && _world.GetCollider(object.ColliderRef).GetCollisionType() == Physics::ColliderCollisionType::Dynamic)
			{
				const auto directionToCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f } - body.Position();

				velocity = directionToCenter.Normalized() * Math::Random::Range(100.f, 300.f);
			}

			body.SetVelocity(velocity);
		}
	}

	// Toggle boxes by pressing B.
	if (Input::IsKeyPressed(SDL_SCANCODE_B))
	{
		_showBoxes = !_showBoxes;
	}

	// Toggle quad trees by pressing Q.
	if (Input::IsKeyPressed(SDL_SCANCODE_Q))
	{
		_showQuadTrees = !_showQuadTrees;
	}
}

void CollisionSample::onUpdate(float deltaTime) noexcept
{
    // Check if the object is outside the screen and if so, bounce it back
    for (auto& object : _objects)
    {
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

void CollisionSample::onRender() noexcept
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

                Display::Draw(circle, object.ObjectColor);
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                const auto rect = collider.GetRectangle() + body.Position();

                Display::Draw(rect, object.ObjectColor);
            }
            break;

            case Math::ShapeType::Polygon:
            case Math::ShapeType::None: break;
        }

		if (_showBoxes)
        {
            Display::DrawBorder(collider.GetBounds(), Color::White(), 2.f);
        }
    }

	if (_showQuadTrees)
	{
		const auto quadTreeBoundaries = _world.GetQuadTreeBoundaries();

		for (const auto& boundary: quadTreeBoundaries)
		{
			Display::DrawBorder(boundary, Color::White(), 2.f);
		}
	}
}

void CollisionSample::createBall() noexcept
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const float minRadius = 10.f;
	const float bounciness = Math::Random::Range(0.f, 1.f);

	auto circle = Math::CircleF{ {0.f, 0.f}, minRadius + bounciness * 20.f };

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);
	_objects.back().ObjectColor = _color;

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * 100.f);

	collider.SetCircle(circle);
	collider.SetBounciness(1.f);
}

void CollisionSample::createBox() noexcept
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
	_objects.back().ObjectColor = _color;

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * 100.f);

	collider.SetRectangle(rect);
	collider.SetBounciness(1.f);
}

void CollisionSample::createWalls() noexcept
{
	constexpr static float wallThickness = 3.f;
	const auto top = Math::RectangleF{
		{0.f, -wallThickness * 0.5f},
		{static_cast<float>(Display::GetWidth()), wallThickness * 0.5f}
	};
	const auto bottom = Math::RectangleF{
		{0.f, static_cast<float>(Display::GetHeight()) - wallThickness * 0.5f},
		{static_cast<float>(Display::GetWidth()), static_cast<float>(Display::GetHeight()) + wallThickness * 0.5f}
	};
	const auto left = Math::RectangleF{
		{-wallThickness * 0.5f, 0.f},
		{wallThickness * 0.5f, static_cast<float>(Display::GetHeight())}
	};
	const auto right = Math::RectangleF{
		{static_cast<float>(Display::GetWidth()) - wallThickness * 0.5f, 0.f},
		{static_cast<float>(Display::GetWidth()) + wallThickness * 0.5f, static_cast<float>(Display::GetHeight())}
	};
	std::array<Math::RectangleF, 4> walls{ top, bottom, left, right };

	// Create walls
	for (const auto& wall : walls)
	{
		// Separate the wall in _wallSplit smaller walls
		for (int i = 0; i < _wallSplit; ++i)
		{
			const auto partWall = Math::RectangleF{
				{wall.MinBound().X + wall.Width() / _wallSplit * i,       wall.MinBound().Y},
				{wall.MinBound().X + wall.Width() / _wallSplit * (i + 1), wall.MaxBound().Y}
			};

			_objects.emplace_back();
			_objects.back().BodyRef = _world.CreateBody();
			_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);
			_objects.back().ObjectColor = _color;

			auto& collider = _world.GetCollider(_objects.back().ColliderRef);
			auto& body = _world.GetBody(_objects.back().BodyRef);

			body.SetPosition(Math::Vec2F::Zero());
			body.SetVelocity(Math::Vec2F::Zero());

			collider.SetRectangle(partWall);
			collider.SetBounciness(1.f);
			collider.SetCollisionType(Physics::ColliderCollisionType::Static);
		}
	}
}

Color CollisionSample::generateRandomColor() noexcept
{
	return Color(
		Math::Random::Range(0, 255),
		Math::Random::Range(0, 255),
		Math::Random::Range(0, 255),
		255
	);
}

void CollisionSample::OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept {}

void CollisionSample::OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	const auto randomColor = generateRandomColor();

    for (auto& object : _objects)
    {
        if (object.ColliderRef == colliderRef || object.ColliderRef == otherColliderRef)
        {
	        object.ObjectColor = randomColor;
        }
    }
}

void CollisionSample::OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	for (auto& _object : _objects)
	{
		if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
		{
			_object.ObjectColor = _triggerStayColor;
		}
	}
}