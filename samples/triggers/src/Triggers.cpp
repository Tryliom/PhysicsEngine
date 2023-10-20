#include "Triggers.h"

#include "Random.h"

Triggers::Triggers()
{
	constexpr static int TriggerBallCount = 50;

	_balls.resize(TriggerBallCount);

	for (int i = 0; i < TriggerBallCount; ++i)
	{
		CreateBall();
	}
}

void Triggers::CreateBall()
{
	const static auto screenWidth = static_cast<float>(Display::GetWidth());
	const static auto screenHeight = static_cast<float>(Display::GetHeight());
	const static auto screenCenter = Math::Vec2F{ screenWidth / 2.f, screenHeight / 2.f };

	_balls.emplace_back();
	_balls.back().BodyRef = _world.CreateBody();
	_balls.back().ColliderRef = _world.CreateCollider(_balls.back().BodyRef);
	_balls.back().Color = Color(155, 100, 100);
	_balls.back().Radius = Math::Random::Range(_ballMinRadius, _ballMaxRadius);

	auto& collider = _world.GetCollider(_balls.back().ColliderRef);
	auto& body = _world.GetBody(_balls.back().BodyRef);

	body.SetPosition({ Math::Random::Range(0.f, screenWidth), Math::Random::Range(0.f, screenHeight) });
	body.SetVelocity((screenCenter - body.Position()).Normalized() * Math::Random::Range(100.f, 300.f));

	collider.SetCircle({ {0.f, 0.f}, _balls.back().Radius });
	collider.SetIsTrigger(true);
	collider.SetContactListener(new BallListener(_balls, _balls.size() - 1));
}

void Triggers::Update(float deltaTime) noexcept
{
	const auto screenWidth = static_cast<float>(Display::GetWidth());
	const auto screenHeight = static_cast<float>(Display::GetHeight());

	// Check if the ball is outside the screen, and if so, teleport it to the other side
	// Set up also his color to normal
	for (auto& ball : _balls)
	{
		auto& body = _world.GetBody(ball.BodyRef);

		ball.Color = _ballColor;

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
	}

	_world.Update(deltaTime);
}

void Triggers::Render() noexcept
{
	for (auto& ball : _balls)
	{
		const auto& body = _world.GetBody(ball.BodyRef);

		if (ball.TriggerEnter)
		{
			Display::DrawCircle(body.Position().X, body.Position().Y, ball.Radius * 1.2f, _ballTriggerEnterColor);
		}
		else if (ball.TriggerExit)
		{
			Display::DrawCircle(body.Position().X, body.Position().Y, ball.Radius * 1.2f, _ballTriggerExitColor);
		}

		Display::DrawCircle(body.Position().X, body.Position().Y, ball.Radius, ball.Color);
	}
}