#include "TriggerListener.h"

TriggerListener::TriggerListener(std::vector<Trigger>& trigger, std::size_t index) noexcept
	: _triggers(trigger),
	  _index(index) {}

void TriggerListener::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_triggers[_index].TriggerEnterTimer = _blinkTimer;
}

void TriggerListener::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_triggers[_index].TriggerExitTimer = _blinkTimer;
}

void TriggerListener::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	_triggers[_index].Color = _triggerStayColor;
}