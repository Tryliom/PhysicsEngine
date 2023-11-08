#include <imgui.h>
#include "CollisionSample.h"

#include "Random.h"
#include "Display.h"
#include "Input.h"

CollisionSample::CollisionSample() noexcept :
	Sample(
		"Collisions",
		"Sample showing how to use collisions with circles and boxes.\n"
        "When a shape:\n"
        "- Starts to collide: it will have a green shape inside.\n"
        "- Stops to collide: it will have a red shape inside.\n"
        "- Stay: it will have a yellow shape inside.\n"
		"Controls:\n"
		"Space -> Freeze/unfreeze all objects\n"
		"B -> Toggle boxes around objects\n"
		"Q -> Toggle quad trees\n"
		"Right click -> Move camera\n"
		"Mouse wheel -> Zoom"
	)
{}

void CollisionSample::onInit() noexcept
{
    Display::SetTitle("Collision Sample");
    _world.SetContactListener(this);

	_objects.resize(_circles + _boxes + 4 * _wallSplit);

    for (int i = 0; i < _circles; ++i)
    {
        createBall();
    }

    for (int i = 0; i < _boxes; ++i)
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

void CollisionSample::onDrawImGui() noexcept
{
    ImGui::DragInt("Circles", &_circles, 1.f, 0, 10000);
    ImGui::DragInt("Boxes", &_boxes, 1.f, 0, 10000);

    if (ImGui::Button("Regenerate"))
    {
        Deinit();
        Init();
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

                if (object.TriggerEnterTimer > 0.f)
                {
                    Display::Draw(circle, _triggerEnterColor, Math::Vec2F::One() * 0.8f);
                }

                if (object.TriggerExitTimer > 0.f)
                {
                    Display::Draw(circle, _triggerExitColor, Math::Vec2F::One() * 0.6f);
                }
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                const auto rect = collider.GetRectangle() + body.Position();

                Display::Draw(rect, object.ObjectColor);

                if (object.TriggerEnterTimer > 0.f)
                {
                    Display::Draw(rect, _triggerEnterColor, Math::Vec2F::One() * 0.8f);
                }

                if (object.TriggerExitTimer > 0.f)
                {
                    Display::Draw(rect, _triggerExitColor, Math::Vec2F::One() * 0.6f);
                }
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
	constexpr static float wallThickness = 10.f;
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
    auto bodyRef = _world.CreateBody();

	// Create walls
	for (const auto& wall : walls)
	{
		// Separate the wall in _wallSplit smaller walls
		for (int i = 0; i < _wallSplit; ++i)
		{
			auto partWall = Math::RectangleF{
				{wall.MinBound().X + wall.Width() / _wallSplit * i,       wall.MinBound().Y},
				{wall.MinBound().X + wall.Width() / _wallSplit * (i + 1), wall.MaxBound().Y}
			};

            if (wall.Width() < wall.Height())
            {
                partWall = Math::RectangleF{
                    {wall.MinBound().X, wall.MinBound().Y + wall.Height() / _wallSplit * i},
                    {wall.MaxBound().X, wall.MinBound().Y + wall.Height() / _wallSplit * (i + 1)}
                };
            }

			_objects.emplace_back();
			_objects.back().BodyRef = bodyRef;
			_objects.back().ColliderRef = _world.CreateCollider(bodyRef);
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

void CollisionSample::OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto& object : _objects)
    {
        if (object.ColliderRef == colliderRef || object.ColliderRef == otherColliderRef)
        {
            object.TriggerEnterTimer = _blinkTimer;
        }
    }
}

void CollisionSample::OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	const auto randomColor = generateRandomColor();

    for (auto& object : _objects)
    {
        if (object.ColliderRef == colliderRef || object.ColliderRef == otherColliderRef)
        {
	        object.ObjectColor = randomColor;
            object.TriggerExitTimer = _blinkTimer;
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