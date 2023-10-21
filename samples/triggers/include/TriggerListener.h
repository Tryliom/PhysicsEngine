#pragma once

#include "ContactListener.h"
#include "Trigger.h"

#include <vector>

class TriggerListener : public Physics::ContactListener
{
public:
	explicit TriggerListener(std::vector<Trigger>& trigger, std::size_t index) noexcept;

private:
	std::vector<Trigger>& _triggers;
	std::size_t _index;

	const Color _triggerStayColor = Color(155, 155, 100);
	const float _blinkTimer = 0.3f;

public:
	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};