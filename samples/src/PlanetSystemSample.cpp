#include "PlanetSystemSample.h"

#include "Random.h"
#include "Input.h"
#include "Display.h"

PlanetSystemSample::PlanetSystemSample() noexcept :
	Sample(
		"Planet System",
		"A simple planet system simulation. Planets are attracted by the suns. Suns are attracted by all the other suns.\n"
		"Controls:\n"
		"Left click -> Create planet\n"
		"Shift + Left click -> Create planet with bigger mass\n"
		"Middle click -> Create sun\n"
		"F -> Follow the nearest sun\n"
		"Right click -> Move camera\n"
		"Mouse wheel -> Zoom\n"
	)
{}

void PlanetSystemSample::onInit() noexcept
{
    Display::SetTitle("Planet System Sample");

    // Create sun
    createSun(Math::Vec2F(0.f, 0.f));

    _followedRef = { true, _suns[0] };

    auto& sunBody = _world.GetBody(_suns[0]);

    sunBody.SetVelocity(Math::Vec2F::One() * 50.f);

    // Create planets
    constexpr int planetsToCreate = 250;
    constexpr float R = 700;

    _planets.reserve(planetsToCreate);

    for (std::size_t i = 0; i < planetsToCreate; i++)
    {
        auto randomAngle = Math::Radian(Math::Degree(Math::Random::Range(0.f, 360.f)));
        auto randomR = Math::Random::Range(R / 2.f, R);

        createPlanet(Math::Vec2F(Math::Cos(randomAngle), Math::Sin(randomAngle)) * randomR + sunBody.Position());
    }
}

void PlanetSystemSample::onDeinit() noexcept
{
    _planets.clear();
    _suns.clear();
}

void PlanetSystemSample::onInput() noexcept
{
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
			createPlanet(Display::GetMousePosition(), Math::Random::Range(1000.f, 3000.f));
		}
	}
	else if (Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
	{
		createPlanet(Display::GetMousePosition() + Math::Vec2F(Math::Random::Range(-_mouseRandomRadius, _mouseRandomRadius),
															   Math::Random::Range(-_mouseRandomRadius, _mouseRandomRadius)));
	}

	if (Input::IsMouseButtonPressed(SDL_BUTTON_MIDDLE))
	{
		createSun(Display::GetMousePosition());
	}
}

void PlanetSystemSample::onUpdate(float deltaTime) noexcept
{
	for (const auto& planet : _planets)
    {
        auto& body = _world.GetBody(planet.BodyRef);

        for (const auto& sun : _suns)
        {
            const auto& sunBody = _world.GetBody(sun);
            Math::Vec2F radius = sunBody.Position() - body.Position();

            if (radius == Math::Vec2F::Zero()) continue;

			const auto& force = radius.Normalized() * (body.Mass() * _sunMass / radius.SquareLength());

            body.AddForce(force * body.Mass());
        }
    }

    for (const auto& sun : _suns)
    {
        auto& sunBody = _world.GetBody(sun);

        for (auto& otherSun : _suns)
        {
            auto& otherSunBody = _world.GetBody(otherSun);

            if (&sunBody == &otherSunBody) continue;

            Math::Vec2F radius = otherSunBody.Position() - sunBody.Position();

            if (radius == Math::Vec2F::Zero()) continue;

			const auto& force = radius.Normalized() * (sunBody.Mass() * otherSunBody.Mass() / radius.SquareLength());

            sunBody.AddForce(force * sunBody.Mass());
        }
    }
}

void PlanetSystemSample::onRender() noexcept
{
	for (const auto& planet : _planets)
    {
        auto& body = _world.GetBody(planet.BodyRef);

	    Display::Draw(Math::CircleF{{body.Position().X, body.Position().Y}, planet.Radius}, planet.PlanetColor);
    }

    for (auto& sun : _suns)
    {
        auto& sunBody = _world.GetBody(sun);

	    Display::Draw(Math::CircleF{{sunBody.Position().X, sunBody.Position().Y}, _sunRadius}, _sunColor);
    }
}

Color PlanetSystemSample::generateRandomColor() noexcept
{
    return Color(
        Math::Random::Range(0, 255),
        Math::Random::Range(0, 255),
        Math::Random::Range(0, 255),
        255
    );
}

void PlanetSystemSample::createPlanet(Math::Vec2F position, float extraMass)
{
    auto planetRef = _world.CreateBody();
    auto& planet = _world.GetBody(planetRef);
    auto mass = Math::Random::Range(800.f, 1400.f) + extraMass;

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
    Math::Vec2F radius = planet.Position() - nearestSunBody.Position();
    // Calculate the velocity needed to make the planet orbit around the center of the screen using his mass.
    Math::Vec2F orbitalVelocity = Math::Vec2F(-radius.Y, radius.X).Normalized() * std::sqrt(nearestSunBody.Mass() * mass / radius.Length());
    // Apply the sun velocity to the planet.
    orbitalVelocity += nearestSunBody.Velocity();

    planet.SetVelocity(orbitalVelocity);

	_planets.push_back({planetRef, generateRandomColor(), mass / 200.f});
}

void PlanetSystemSample::createSun(Math::Vec2F position)
{
    const auto& sunRef = _world.CreateBody();
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