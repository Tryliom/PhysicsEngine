#pragma once

#include "World.h"
#include "Sample.h"
#include "Color.h"

struct Object
{
	Physics::BodyRef BodyRef{};
	Physics::ColliderRef ColliderRef{};

    float TriggerEnterTimer{};
    float TriggerExitTimer{};
    Color Color;
};

class TriggerSample final : public Sample, Physics::ContactListener
{
private:
	std::vector<Object> _objects;

    static constexpr Color _color = Color(100, 100, 100);
    static constexpr Color _triggerEnterColor = Color(100, 255, 100);
    static constexpr Color _triggerExitColor = Color(255, 100, 100);
    static constexpr Color _triggerStayColor = Color(155, 155, 100);
    static constexpr float _blinkTimer = 0.3f;

    void onInit() noexcept override;
    void onDeinit() noexcept override;

	void onUpdate(float deltaTime) noexcept override;
	void onRender() noexcept override;

    void createBall() noexcept;
	void createBox() noexcept;
	void createPolygon() noexcept;

public:
    void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
    void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
    void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};