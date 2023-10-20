#pragma once

#include "BallListener.h"
#include "World.h"

class Triggers
{
public:
	Triggers();

private:
	Physics::World _world;
	std::vector<Ball> _balls;

	const float _ballMinRadius = 10.f;
	const float _ballMaxRadius = 30.f;

	const Color _ballColor = Color(155, 100, 100);
	const Color _ballTriggerEnterColor = Color(100, 155, 100);
	const Color _ballTriggerExitColor = Color(100, 100, 155);

	void CreateBall();

public:
	void Update(float deltaTime) noexcept;
	void Render() noexcept;
};