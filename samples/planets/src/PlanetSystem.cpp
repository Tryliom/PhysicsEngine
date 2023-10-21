#include "PlanetSystem.h"

#include "Random.h"
#include "Input.h"

#include <iostream>

PlanetSystem::PlanetSystem() : _world(1000)
{
    // Create sun
    CreateSun(Math::Vec2F(0.f, 0.f));

    _followedRef = { true, _suns[0] };

    auto& sunBody = _world.GetBody(_suns[0]);

    sunBody.SetVelocity(Math::Vec2F::One() * 50.f);

    // Create planets
    constexpr int planetsToCreate = 250;
    constexpr float R = 700;

    _planets.resize(planetsToCreate);

    for (std::size_t i = 0; i < planetsToCreate; i++)
    {
        auto randomAngle = Math::Radian(Math::Degree(Math::Random::Range<float>(0.f, 360.f)));
        auto randomR = Math::Random::Range<float>(R / 2.f, R);

        CreatePlanet(Math::Vec2F(Math::Cos(randomAngle), Math::Sin(randomAngle)) * randomR + sunBody.Position());
    }
}

void PlanetSystem::Update(float deltaTime) noexcept
{
    _world.Update(deltaTime);

    if (Input::IsKeyPressed(SDL_SCANCODE_F))
    {
        if (_followedRef.Enable)
        {
            _followedRef.Enable = false;
        }
        else
        {
            _followedRef.Enable = true;

            // Get the nearest sun.
            auto nearestSun = _suns[0];
            auto mousePosition = Display::GetMousePosition();

            for (auto& sun : _suns)
            {
                auto sunBody = _world.GetBody(sun);
                auto nearestSunBody = _world.GetBody(nearestSun);

                if ((sunBody.Position() - mousePosition).Length() < (nearestSunBody.Position() - mousePosition).Length())
                {
                    nearestSun = sun;
                }
            }

            _followedRef.Ref = nearestSun;

            Display::LookAt(_world.GetBody(_followedRef.Ref).Position());
        }
    }


    auto mouseWheelDelta = Input::GetMouseWheelDelta();

    if (mouseWheelDelta != 0)
    {
        Math::Vec2F target = Display::GetMousePosition();

        if (_followedRef.Enable)
        {
            target = _world.GetBody(_followedRef.Ref).Position();
        }

        Display::SetCameraZoom(Display::GetCameraZoom() + mouseWheelDelta * 0.05f, target);
    }

    auto mouseDelta = Display::GetMouseDelta();

    // Move camera when holding right mouse button.
    if (Input::IsMouseButtonHeld(SDL_BUTTON_RIGHT) && mouseDelta != Math::Vec2F::Zero())
    {
        Display::MoveCamera(mouseDelta);

        _followedRef.Enable = false;
    }

    if (_followedRef.Enable)
    {
        auto& followedSunBody = _world.GetBody(_followedRef.Ref);

        Display::LookAt(followedSunBody.Position());
    }

    if (Input::IsKeyHeld(SDL_SCANCODE_LSHIFT))
    {
        if (Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
        {
            CreatePlanet(Display::GetMousePosition(), Math::Random::Range<float>(1000.f, 3000.f));
        }
    }
    else if (Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
    {
        CreatePlanet(Display::GetMousePosition() + Math::Vec2F(Math::Random::Range<float>(-_mouseRandomRadius, _mouseRandomRadius), Math::Random::Range<float>(-_mouseRandomRadius, _mouseRandomRadius)));
    }

    if (Input::IsMouseButtonPressed(SDL_BUTTON_MIDDLE))
    {
        CreateSun(Display::GetMousePosition());
    }

    for (auto& planet : _planets)
    {
        auto& body = _world.GetBody(planet.BodyRef);

        for (auto sun : _suns)
        {
            auto& sunBody = _world.GetBody(sun);

            // Apply force to planet to make it orbit around the center of the screen.
            Math::Vec2F centerToPlanet = sunBody.Position() - body.Position();

            if (centerToPlanet == Math::Vec2F::Zero()) continue;

            body.ApplyForce(centerToPlanet.Normalized() * (_sunMass * body.Mass() / (centerToPlanet.Length() * centerToPlanet.Length())));
        }
    }

    for (auto& sun : _suns)
    {
        auto& sunBody = _world.GetBody(sun);

        for (auto& otherSun : _suns)
        {
            auto& otherSunBody = _world.GetBody(otherSun);

            if (&sunBody == &otherSunBody) continue;

            Math::Vec2F sunToOtherSun = otherSunBody.Position() - sunBody.Position();

            if (sunToOtherSun == Math::Vec2F::Zero()) continue;

            sunBody.ApplyForce(sunToOtherSun.Normalized() * (otherSunBody.Mass() *
                sunBody.Mass() / (sunToOtherSun.Length() * sunToOtherSun.Length())));
        }
    }
}

void PlanetSystem::Render() noexcept
{
	for (auto& planet : _planets)
    {
        auto& body = _world.GetBody(planet.BodyRef);

	    Display::Draw(Math::CircleF{{body.Position().X, body.Position().Y}, planet.Radius}, planet.Color);
    }

    for (auto& sun : _suns)
    {
        auto& sunBody = _world.GetBody(sun);

	    Display::Draw(Math::CircleF{{sunBody.Position().X, sunBody.Position().Y}, _sunRadius}, _sunColor);
    }
}

Color PlanetSystem::GenerateRandomColor() noexcept
{
    return Color(
        Math::Random::Range<int>(0, 255),
        Math::Random::Range<int>(0, 255),
        Math::Random::Range<int>(0, 255),
        255
    );
}

void PlanetSystem::CreatePlanet(Math::Vec2F position, float extraMass)
{
    auto planetRef = _world.CreateBody();
    auto& planet = _world.GetBody(planetRef);
    auto mass = Math::Random::Range<float>(800.f, 1400.f) + extraMass;
    _planets.emplace_back(planetRef, GenerateRandomColor(), mass / 200.f);

    planet.SetPosition(position);
    planet.SetMass(mass);

    // Get the nearest sun.
    auto nearestSun = _suns[0];

    for (auto& sun : _suns)
    {
        auto sunBody = _world.GetBody(sun);
        auto nearestSunBody = _world.GetBody(nearestSun);

        if ((sunBody.Position() - planet.Position()).Length() < (nearestSunBody.Position() - planet.Position()).Length())
        {
            nearestSun = sun;
        }
    }

    auto& nearestSunBody = _world.GetBody(nearestSun);

    // Make the planet velocity perpendicular to the vector from the center of the screen to the planet.
    Math::Vec2F sunToPlanet = planet.Position() - nearestSunBody.Position();
    // Calculate the velocity needed to make the planet orbit around the center of the screen using his mass.
    Math::Vec2F orbitalVelocity = Math::Vec2F(-sunToPlanet.Y, sunToPlanet.X).Normalized() * std::sqrt(nearestSunBody.Mass() * mass / sunToPlanet.Length());
    // Apply the sun velocity to the planet.
    orbitalVelocity += nearestSunBody.Velocity();

    planet.SetVelocity(orbitalVelocity);
}

void PlanetSystem::CreateSun(Math::Vec2F position)
{
    auto sunRef = _world.CreateBody();
    auto& sun = _world.GetBody(sunRef);

    _suns.push_back(sunRef);
    sun.SetPosition(position);
    sun.SetMass(_sunMass);

    if (_suns.size() == 1) return;

    // Make the sun velocity perpendicular to the vector from the nearest sun.
    auto& nearestSun = _suns[0];

    for (auto& otherSun : _suns)
    {
        if (otherSun == nearestSun || otherSun == sunRef) continue;

        auto otherSunBody = _world.GetBody(otherSun);
        auto nearestSunBody = _world.GetBody(nearestSun);

        if ((otherSunBody.Position() - sun.Position()).Length() < (nearestSunBody.Position() - sun.Position()).Length())
        {
            nearestSun = otherSun;
        }
    }

    auto nearestSunBody = _world.GetBody(nearestSun);
    Math::Vec2F centerToSun = nearestSunBody.Position() - sun.Position();

    // Calculate the velocity needed to make the sun orbit around the center of the screen using his mass.
    Math::Vec2F orbitalVelocity = Math::Vec2F(-centerToSun.Y, centerToSun.X).Normalized() * std::sqrt(_sunMass * _sunMass / centerToSun.Length());
    // Apply the sun velocity to the sun.
    orbitalVelocity += nearestSunBody.Velocity();

    sun.SetVelocity(orbitalVelocity);
}