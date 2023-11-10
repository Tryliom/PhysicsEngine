#pragma once
#include "Sample.h"

#include "Body.h"
#include "Vec2.h"
#include "World.h"
#include "Color.h"

#include <vector>

struct Planet
{
    Physics::BodyRef BodyRef {};
    Color PlanetColor;
    float Radius {};
};

struct Follow
{
    bool Enable;
    Physics::BodyRef Ref;
};

class PlanetSystemSample final : public Sample
{
public:
	PlanetSystemSample() noexcept;

private:
    static constexpr float _mouseRandomRadius = 15.f;
	static constexpr float _sunMass = 100000.f;
	static constexpr float _sunRadius = 10.f;
	static constexpr Color _sunColor = Color(255, 255, 0);

    std::vector<Planet> _planets;
    std::vector<Physics::BodyRef> _suns;

    Follow _followedRef {};

private:
    void onInit() noexcept override;
    void onDeinit() noexcept override;

	void onInput() noexcept override;
    void onUpdate(float deltaTime) noexcept override;
    void onRender() noexcept override;
    void onDrawImGui() noexcept override {}

    static Color generateRandomColor() noexcept;

    void createPlanet(Math::Vec2F position, float extraMass = 0.f);
    void createSun(Math::Vec2F position);
};