#pragma once

#include "World.h"
#include "Sample.h"
#include "Color.h"

class CollisionSample final : public Sample, Physics::ContactListener
{
	struct Object
	{
		Physics::BodyRef BodyRef{};
		Physics::ColliderRef ColliderRef{};

	    float TriggerEnterTimer{};
	    float TriggerExitTimer{};
	    Color ObjectColor;
	};

public:
	CollisionSample() noexcept;

private:
	std::vector<Object> _objects;

	bool _stop {};
	bool _showBoxes {};
	bool _showQuadTrees {};

    static constexpr Color _color = Color(100, 100, 100);
    static constexpr Color _triggerEnterColor = Color(100, 255, 100);
    static constexpr Color _triggerExitColor = Color(255, 100, 100);
    static constexpr Color _triggerStayColor = Color(155, 155, 100);
    static constexpr float _blinkTimer = 0.3f;
	static constexpr int _wallSplit = 50;

    void onInit() noexcept override;
    void onDeinit() noexcept override;

	void onUpdate(float deltaTime) noexcept override;
	void onRender() noexcept override;
	void onInput() noexcept override;

	void createBox() noexcept;
	void createWalls() noexcept;
    void createBall() noexcept;

	static Color generateRandomColor() noexcept;

public:
    void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
    void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
    void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}

	void OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};