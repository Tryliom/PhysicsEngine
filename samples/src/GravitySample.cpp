#include <imgui.h>
#include "GravitySample.h"

#include "Random.h"
#include "Display.h"
#include "Input.h"

GravitySample::GravitySample() noexcept :
	Sample(
		"Gravity",
		"Sample showing how gravity work with a ground.\n"
		"Controls:\n"
		"Left click -> Spawn shape\n"
        "Right click -> Change shape\n"
        "Mouse wheel -> Change shape size\n"
        "R -> Clear all shapes"
	)
{}

void GravitySample::onInit() noexcept
{
    Display::SetTitle("Gravity Sample");
    _world.SetGravity(_gravity);

	createGround();
}

void GravitySample::onDeinit() noexcept
{
    _objects.clear();
}

void GravitySample::onInput() noexcept
{
	const auto mouseWheelDelta = Input::GetMouseWheelDelta();

	if (mouseWheelDelta != 0)
	{
		_shapeSize += mouseWheelDelta * 0.5f;

        if (_shapeSize < 1.f)
        {
            _shapeSize = 1.f;
        }
	}

    if (Input::IsKeyPressed(SDL_SCANCODE_R))
    {
        _objects.clear();
    }

    if (Input::IsMouseButtonPressed(SDL_BUTTON_LEFT))
    {
        switch (_shapeType)
        {
            case Math::ShapeType::Circle:
            {
                createBall();
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                createBox();
            }
            break;

            case Math::ShapeType::Polygon:
            case Math::ShapeType::None: break;
        }
    }

    if (Input::IsMouseButtonPressed(SDL_BUTTON_RIGHT))
    {
        switch (_shapeType)
        {
            case Math::ShapeType::Circle:
            {
                _shapeType = Math::ShapeType::Rectangle;
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                _shapeType = Math::ShapeType::Circle;
            }
            break;

            case Math::ShapeType::Polygon:
            case Math::ShapeType::None: break;
        }
    }
}

void GravitySample::onDrawImGui() noexcept
{
    float gravityX = _gravity.X;
    float gravityY = _gravity.Y;

    ImGui::DragFloat("Gravity X", &gravityX, 1.f, -500, 500);
    ImGui::DragFloat("Gravity Y", &gravityY, 1.f, -500, 500);

    _gravity = {gravityX, gravityY};

    if (ImGui::Button("Reset gravity"))
    {
        _gravity = Math::Vec2F::Down() * 9.81f;
    }

    _world.SetGravity(_gravity * -1);

    ImGui::DragFloat("Bounciness", &_objectsBounciness, 0.01f, 0.f, 1.f);

    if (ImGui::Button("Regenerate"))
    {
        Deinit();
        Init();
    }
}

void GravitySample::onRender() noexcept
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

                Display::Draw(circle, _shapeColor);
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                const auto rect = collider.GetRectangle() + body.Position();

                Display::Draw(rect, _shapeColor);
            }
            break;

            case Math::ShapeType::Polygon:
            case Math::ShapeType::None: break;
        }
    }

    const auto& groundBody = _world.GetBody(_ground.BodyRef);
    const auto& groundCollider = _world.GetCollider(_ground.ColliderRef);

    Display::Draw(groundCollider.GetRectangle() + groundBody.Position(), _groundColor);

    const Math::Vec2F mousePosition = static_cast<Math::Vec2F>(Input::GetMousePosition());

    switch (_shapeType)
    {
        case Math::ShapeType::Circle:
        {
            Display::Draw(Math::CircleF{mousePosition, _shapeSize}, _mouseShapeColor);
        }
        break;

        case Math::ShapeType::Rectangle:
        {
            Display::Draw(Math::RectangleF::FromCenter(mousePosition, Math::Vec2F::One() * 0.5f * _shapeSize), _mouseShapeColor);
        }
        break;

        case Math::ShapeType::Polygon:
        case Math::ShapeType::None: break;
    }
}

void GravitySample::createBall() noexcept
{
	auto circle = Math::CircleF{ {0.f, 0.f}, _shapeSize };

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition(Display::GetMousePosition());
	body.SetUseGravity(true);

	collider.SetCircle(circle);
	collider.SetBounciness(_objectsBounciness);
}

void GravitySample::createBox() noexcept
{
	auto rect = Math::RectangleF{
		Math::Vec2F::One() * 0.5f * -_shapeSize,
        Math::Vec2F::One() * 0.5f * _shapeSize
	};

	_objects.emplace_back();
	_objects.back().BodyRef = _world.CreateBody();
	_objects.back().ColliderRef = _world.CreateCollider(_objects.back().BodyRef);

	auto& collider = _world.GetCollider(_objects.back().ColliderRef);
	auto& body = _world.GetBody(_objects.back().BodyRef);

	body.SetPosition(Display::GetMousePosition());
    body.SetUseGravity(true);

	collider.SetRectangle(rect);
	collider.SetBounciness(_objectsBounciness);
}

void GravitySample::createGround() noexcept
{
	constexpr static float wallThickness = 30.f;
	const auto bottom = Math::RectangleF{
		{static_cast<float>(Display::GetWidth()) * 0.25f, static_cast<float>(Display::GetHeight()) - wallThickness * 0.5f},
		{static_cast<float>(Display::GetWidth()) * 0.75f, static_cast<float>(Display::GetHeight()) + wallThickness * 0.5f}
	};
    auto bodyRef = _world.CreateBody();

    _ground.BodyRef = bodyRef;
    _ground.ColliderRef = _world.CreateCollider(bodyRef);

    auto& collider = _world.GetCollider(_ground.ColliderRef);
    auto& body = _world.GetBody(_ground.BodyRef);

    body.SetPosition({0, 0});
    body.SetBodyType(Physics::BodyType::Static);

    collider.SetRectangle(bottom);
    collider.SetBounciness(_objectsBounciness);
}