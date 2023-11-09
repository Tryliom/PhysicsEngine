#include "FreeTriggerSample.h"

#include "Display.h"
#include "Input.h"

FreeTriggerSample::FreeTriggerSample() noexcept :
	Sample(
		"Free Triggers",
		"Sample letting you create circle and rectangle triggers.\n"
		"Controls:\n"
		"1 -> Choose circle shape\n"
		"2 -> Choose rectangle shape\n"
		"Mouse wheel -> Change shape size\n"
		"Left mouse button -> Create shape\n"
		"R -> Remove all shapes\n"
		"Right mouse button -> Move camera\n"
		"Q -> Toggle quad tree rendering"
	)
{}

void FreeTriggerSample::onInit() noexcept
{
	Display::SetTitle("Free Trigger Sample");
	_world.SetContactListener(this);
}

void FreeTriggerSample::onDeinit() noexcept
{
	_objects.clear();
}

void FreeTriggerSample::onInput() noexcept
{
	const auto mouseWheelDelta = Input::GetMouseWheelDelta();

	if (mouseWheelDelta != 0)
	{
		if (mouseWheelDelta > 0)
		{
			_shapeSize += 0.5f;
		}
		else
		{
			_shapeSize -= 0.5f;
		}

		_shapeSize = Math::Max(_shapeSize, 0.5f);
	}

	if (Input::IsKeyPressed(SDL_SCANCODE_1))
	{
		_shapeType = Math::ShapeType::Circle;
	}
	else if (Input::IsKeyPressed(SDL_SCANCODE_2))
	{
		_shapeType = Math::ShapeType::Rectangle;
	}

	if (Input::IsKeyPressed(SDL_SCANCODE_R))
	{
		for (auto& object : _objects)
		{
			_world.DestroyBody(object.BodyRef);
		}

		_objects.clear();
	}

	if (Input::IsKeyPressed(SDL_SCANCODE_Q))
	{
		_showQuadTrees = !_showQuadTrees;
	}

	if (Input::IsMouseButtonPressed(SDL_BUTTON_LEFT))
	{
		if (_shapeType == Math::ShapeType::Circle)
		{
			createBall();
		}
		else if (_shapeType == Math::ShapeType::Rectangle)
		{
			createBox();
		}
	}

	if (Input::IsMouseButtonPressed(SDL_BUTTON_RIGHT))
	{
		Display::MoveCamera(Display::GetMouseDelta());
	}
}

void FreeTriggerSample::onUpdate(float deltaTime) noexcept
{
	for (auto& object : _objects)
	{
		if (object.TriggerEnterTimer > 0.f)
		{
			object.TriggerEnterTimer -= deltaTime;
		}

		if (object.TriggerExitTimer > 0.f)
		{
			object.TriggerExitTimer -= deltaTime;
		}
	}
}

void FreeTriggerSample::onRender() noexcept
{
	for (auto& object : _objects)
	{
		const auto& body = _world.GetBody(object.BodyRef);
		const auto& collider = _world.GetCollider(object.ColliderRef);

		switch (collider.GetShapeType())
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
			default: break;
		}
	}

	if (_showQuadTrees)
	{
		const auto quadTreeBoundaries = _world.GetQuadTreeBoundaries();

		for (const auto& boundary: quadTreeBoundaries)
		{
			Display::DrawBorder(boundary, Color::White(), 1.f);
		}
	}

	switch(_shapeType)
	{
		case Math::ShapeType::Circle:
		{
			Display::Draw({Display::GetMousePosition(), _shapeSize}, Color::Green());
		}
		break;

		case Math::ShapeType::Rectangle:
		{
			auto rect = Math::RectangleF::FromCenter(Display::GetMousePosition(), Math::Vec2F::One() * _shapeSize / 2.f);

			Display::Draw(rect, Color::Green());
		}
		break;
		case Math::ShapeType::Polygon:
		case Math::ShapeType::None:break;
	}
}

void FreeTriggerSample::createBall() noexcept
{
	const auto mousePosition = Display::GetMousePosition();

	auto bodyRef = _world.CreateBody();
	auto& body = _world.GetBody(bodyRef);
	auto colliderRef = _world.CreateCollider(bodyRef);
	auto& collider = _world.GetCollider(colliderRef);

	body.SetPosition(mousePosition);
	collider.SetCircle(Math::CircleF(Math::Vec2F::Zero(), _shapeSize));
	collider.SetIsTrigger(true);

	_objects.push_back({bodyRef, colliderRef, 0.f, 0.f, _color});
}

void FreeTriggerSample::createBox() noexcept
{
	const auto mousePosition = Display::GetMousePosition();

	auto bodyRef = _world.CreateBody();
	auto& body = _world.GetBody(bodyRef);
	auto colliderRef = _world.CreateCollider(bodyRef);
	auto& collider = _world.GetCollider(colliderRef);

	body.SetPosition(mousePosition);
	collider.SetRectangle(Math::RectangleF::FromCenter(Math::Vec2F::Zero(), Math::Vec2F::One() * _shapeSize / 2.f));
	collider.SetIsTrigger(true);

	_objects.push_back({bodyRef, colliderRef, 0.f, 0.f, _color});
}

void FreeTriggerSample::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept {}

void FreeTriggerSample::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept {}

void FreeTriggerSample::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	for (auto& _object : _objects)
	{
		if (_object.ColliderRef == colliderRef || _object.ColliderRef == otherColliderRef)
		{
			_object.ObjectColor = _triggerStayColor;
		}
	}
}