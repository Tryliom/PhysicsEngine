#pragma once

#include "ContactListener.h"
#include "Ball.h"

#include <vector>

class BallListener : public Physics::ContactListener
{
public:
	explicit BallListener(std::vector<Ball>& balls, std::size_t ballIndex) noexcept;

private:
	std::vector<Ball>& _balls;
	std::size_t _ballIndex;

	const Color _ballTriggerEnterColor = Color(100, 155, 100);
	const Color _ballTriggerExitColor = Color(100, 100, 155);
	const Color _ballTriggerStayColor = Color(155, 155, 100);

public:
	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};