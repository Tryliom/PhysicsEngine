#pragma once

#include "Ref.h"
#include "Display.h"

struct Ball
{
	Physics::BodyRef BodyRef;
	Physics::ColliderRef ColliderRef;
	Color Color;
	float Radius;
	bool TriggerEnter;
	bool TriggerExit;
};