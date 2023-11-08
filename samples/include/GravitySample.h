#pragma once

#include "World.h"
#include "Sample.h"
#include "Color.h"

class GravitySample final : public Sample
{
	struct Object
	{
		Physics::BodyRef BodyRef{};
		Physics::ColliderRef ColliderRef{};
	};

public:
    GravitySample() noexcept;

private:
	std::vector<Object> _objects;
    Object _ground;
    Math::Vec2F _gravity = Math::Vec2F::Down() * 9.81f;
    float _objectsBounciness = 1.f;

    Math::ShapeType _shapeType = Math::ShapeType::Circle;
    float _shapeSize = 20.f;

    static constexpr Color _shapeColor = Color(200, 200, 200);
    static constexpr Color _mouseShapeColor = Color(31, 150, 184);
    static constexpr Color _groundColor = Color(109, 75, 20);

    void onInit() noexcept override;
    void onDeinit() noexcept override;

	void onUpdate(float deltaTime) noexcept override {}
	void onRender() noexcept override;
	void onInput() noexcept override;
    void onDrawImGui() noexcept override;

	void createBox() noexcept;
    void createBall() noexcept;
	void createGround() noexcept;
};