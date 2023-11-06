#pragma once

#include "World.h"
#include "Sample.h"
#include "Color.h"

class FreeTriggerSample final : public Sample, Physics::ContactListener
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
	FreeTriggerSample() noexcept;

private:
	std::vector<Object> _objects;

	Math::ShapeType _shapeType { Math::ShapeType::Circle };
	float _shapeSize { 5.f };

	bool _showQuadTrees {};

	static constexpr Color _color = Color(100, 100, 100);
	static constexpr Color _triggerEnterColor = Color(100, 255, 100);
	static constexpr Color _triggerExitColor = Color(255, 100, 100);
	static constexpr Color _triggerStayColor = Color(155, 155, 100);
	static constexpr float _blinkTimer = 0.3f;

	void onInit() noexcept override;
	void onDeinit() noexcept override;

	void onInput() noexcept override;
	void onUpdate(float deltaTime) noexcept override;
	void onRender() noexcept override;

	void createBall() noexcept;
	void createBox() noexcept;

public:
	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
};