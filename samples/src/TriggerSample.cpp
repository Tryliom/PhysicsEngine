#include <imgui.h>
#include "TriggerSample.h"

#include "Random.h"
#include "Display.h"
#include "Input.h"

TriggerSample::TriggerSample() noexcept :
	Sample(
		"Triggers",
		"Sample showing how to use triggers with circles, polygons and boxes.\n"
        "When a shape:\n"
        "- Starts to collide: it will have a green shape outside.\n"
        "- Stops colliding: it will have a red shape outside.\n"
        "- Is colliding: it will have a yellow shape outside.\n"
		"Controls:\n"
		"Space -> Freeze/unfreeze all objects\n"
		"B -> Toggle boxes around objects\n"
		"Q -> Toggle quad trees\n"
		"Right click -> Move camera\n"
		"Mouse wheel -> Zoom"
	)
{}

void TriggerSample::onInit() noexcept
{
    Display::SetTitle("Trigger Sample");
    _world.SetContactListener(this);

	_objects.resize(_circles + _boxes + _polygons);

	for (int i = 0; i < _circles; ++i)
	{
        createBall();
	}

    for (int i = 0; i < _boxes; ++i)
    {
        createBox();
    }

    for (int i = 0; i < _polygons; ++i)
    {
        createPolygon();
    }

	_mouseObject.BodyRef = _world.CreateBody();
	_mouseObject.ColliderRef = _world.CreateCollider(_mouseObject.BodyRef);

	auto& collider = _world.GetCollider(_mouseObject.ColliderRef);

	collider.SetCircle({ {0.f, 0.f}, 3.f });
	collider.SetIsTrigger(true);

	_world.GetBody(_mouseObject.BodyRef).SetPosition({ 0.f, 0.f });
}

void TriggerSample::onDeinit() noexcept
{
    _objects.clear();
}

void TriggerSample::onInput() noexcept
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

			if (!_stop)
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

void TriggerSample::onDrawImGui() noexcept
{
    ImGui::DragInt("Circles", &_circles, 1, 0, 1000);
    ImGui::DragInt("Boxes", &_boxes, 1, 0, 1000);
    ImGui::DragInt("Polygons", &_polygons, 1, 0, 1000);

    if (ImGui::Button("Regenerate"))
    {
        Deinit();
        Init();
    }
}

void TriggerSample::onUpdate(float deltaTime) noexcept
{
	const auto screenWidth = static_cast<float>(Display::GetWidth());
	const auto screenHeight = static_cast<float>(Display::GetHeight());

    // Check if the object is outside the screen and if so, bounce it back
    for (auto& object : _objects)
    {
        auto& body = _world.GetBody(object.BodyRef);

        if (body.Position().X < 0.f || body.Position().X > screenWidth)
        {
            const auto velocity = body.Velocity();
            body.SetVelocity({ -velocity.X, velocity.Y });
        }

        if (body.Position().Y < 0.f || body.Position().Y > screenHeight)
        {
            const auto velocity = body.Velocity();
            body.SetVelocity({ velocity.X, -velocity.Y });
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

	// Update the mouse object
	auto& mouseBody = _world.GetBody(_mouseObject.BodyRef);

	mouseBody.SetPosition(Display::GetMousePosition());
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

                Display::Draw(circle, object.ObjectColor);
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

                Display::Draw(rect, object.ObjectColor);
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

                Display::Draw(poly, object.ObjectColor);
            }
            case Math::ShapeType::None: break;
        }

		if (_showBoxes)
        {
            Display::DrawBorder(collider.GetBounds(), Color::White(), 2.f);
        }
    }

	// Draw the mouse object
	const auto& mouseBody = _world.GetBody(_mouseObject.BodyRef);
	const auto& mouseCollider = _world.GetCollider(_mouseObject.ColliderRef);

	Display::Draw(mouseCollider.GetCircle() + mouseBody.Position(), Color::White());
	if (_showBoxes) Display::DrawBorder(mouseCollider.GetBounds(), Color::White(), 2.f);

    // Clear object color
    for (auto& object : _objects)
    {
        object.ObjectColor = _color;
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

void TriggerSample::createBall() noexcept
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const float minRadius = 10.f;
	const float maxRadius = 30.f;

	auto circle = Math::CircleF{ {0.f, 0.f}, Math::Random::Range(minRadius, maxRadius) };

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity({ Math::Random::Range(-300.f, 300.f), Math::Random::Range(-300.f, 300.f) });

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
    body.SetVelocity({ Math::Random::Range(-300.f, 300.f), Math::Random::Range(-300.f, 300.f) });

	collider.SetRectangle(rect);
	collider.SetIsTrigger(true);
}

void TriggerSample::createPolygon() noexcept
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };
	const float min = 10.f;
	const float max = 70.f;

	std::vector<Math::Vec2F> vertices;

	vertices.resize(3);

	vertices.emplace_back(min, min);
    vertices.emplace_back(max, min);
    vertices.emplace_back(max, max);

	auto poly = Math::PolygonF{ vertices };

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
    body.SetVelocity({ Math::Random::Range(-300.f, 300.f), Math::Random::Range(-300.f, 300.f) });

	collider.SetPolygon(poly);
	collider.SetIsTrigger(true);
}

void TriggerSample::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto& _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.TriggerEnterTimer = _blinkTimer;
        }
    }
}

void TriggerSample::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto& _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.TriggerExitTimer = _blinkTimer;
        }
    }
}

void TriggerSample::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
    for (auto& _object : _objects)
    {
        if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
        {
            _object.ObjectColor = _triggerStayColor;
        }
    }
}
