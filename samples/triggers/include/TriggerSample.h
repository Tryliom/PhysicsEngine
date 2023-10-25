#pragma once

#include "World.h"
#include "Display.h"

struct Object
{
	Physics::BodyRef BodyRef{};
	Physics::ColliderRef ColliderRef{};

    float TriggerEnterTimer{};
    float TriggerExitTimer{};
    Color Color;
};

class TriggerSample : public Physics::ContactListener
{
public:
	TriggerSample();

private:
	Physics::World _world;
	std::vector<Object> _objects;

    static constexpr Color _color = Color(100, 100, 100);
    static constexpr Color _triggerEnterColor = Color(100, 255, 100);
    static constexpr Color _triggerExitColor = Color(255, 100, 100);
    static constexpr Color _triggerStayColor = Color(155, 155, 100);
    static constexpr float _blinkTimer = 0.3f;

	void CreateBall();
	void CreateBox();
	void CreatePolygon();

public:
	void Update(float deltaTime) noexcept;
	void Render() noexcept;

    void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
    void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
    void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};