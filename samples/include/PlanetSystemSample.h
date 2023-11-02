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

class PlanetSystemSample final : public Sample, Physics::ContactListener
{
private:
    const float _mouseRandomRadius = 15.f;
    const float _sunMass = 10000.f;
    const float _sunRadius = 10.f;
    const Color _sunColor = Color(255, 255, 0);

    std::vector<Planet> _planets;
    std::vector<Physics::BodyRef> _suns;

    Follow _followedRef {};

private:
    void onInit() noexcept override;
    void onDeinit() noexcept override;

    void onUpdate(float deltaTime) noexcept override;
    void onRender() noexcept override;

    static Color generateRandomColor() noexcept;

    void createPlanet(Math::Vec2F position, float extraMass = 0.f);
    void createSun(Math::Vec2F position);
};