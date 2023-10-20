#include "BallListener.h"

BallListener::BallListener(std::vector<Ball>& balls, std::size_t ballIndex) noexcept
	: _balls(balls),
	_ballIndex(ballIndex) {}

void BallListener::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_balls[_ballIndex].Color = _ballTriggerEnterColor;
	_balls[_ballIndex].TriggerEnter = true;
	_balls[_ballIndex].TriggerExit = false;
}

void BallListener::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_balls[_ballIndex].Color = _ballTriggerExitColor;
	_balls[_ballIndex].TriggerExit = true;
	_balls[_ballIndex].TriggerEnter = false;
}

void BallListener::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_balls[_ballIndex].Color = _ballTriggerStayColor;
}