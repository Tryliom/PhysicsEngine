#pragma once

#include "Body.h"
#include "Vec2.h"
#include "World.h"
#include "Display.h"

#include <vector>

struct Planet
{
    Physics::BodyRef BodyRef {};
    Color Color;
    float Radius {};
};

struct Follow
{
    bool Enable;
    Physics::BodyRef Ref;
};

class PlanetSystem
{
public:
    PlanetSystem();

private:
    Physics::World _world;

    const float _mouseRandomRadius = 15.f;
    const float _sunMass = 10000.f;
    const float _sunRadius = 10.f;
    const Color _sunColor = Color(255, 255, 0);

    std::vector<Planet> _planets;
    std::vector<Physics::BodyRef> _suns;

    Follow _followedRef {};

public:
    void Update(float deltaTime) noexcept;
    void Render() noexcept;

    static Color GenerateRandomColor() noexcept;

    void CreatePlanet(Math::Vec2F position, float extraMass = 0.f);
    void CreateSun(Math::Vec2F position);
};