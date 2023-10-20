#pragma once

#include "Ref.h"

namespace Physics
{
	class ContactListener
	{
	public:
		virtual void OnTriggerEnter(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
		virtual void OnTriggerExit(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
		virtual void OnTriggerStay(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
	};
}