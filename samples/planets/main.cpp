#include "Display.h"

#include "Body.h"
#include "World.h"
#include "Vec2.h"
#include "Input.h"
#include "Random.h"

struct Planet
{
	BodyRef BodyRef;
	Color Color;
	float Radius;
};

constexpr int SCREEN_WIDTH = 1550;
constexpr int SCREEN_HEIGHT = 900;

constexpr float MouseRandomRadius = 15.f;
std::vector<Planet> Planets;

constexpr float SunMass = 100000.f;
constexpr int SunRadius = 10;
constexpr Color SunColor = Color(255, 255, 0);
std::vector<Vec2F> Suns;

constexpr bool PlanetsInteract = false;

Color GenerateRandomColor()
{
	return Color(Random::Range<int>(0, 255), Random::Range<int>(0, 255), Random::Range<int>(0, 255));
}

void CreatePlanet(Vec2F position, float extraMass = 0.f)
{
	auto planetRef = World::CreateBody();
	auto& planet = World::GetBody(planetRef);
	auto mass = Random::Range<float>(800.f, 1400.f) + extraMass;
	Planets.emplace_back(planetRef, GenerateRandomColor(), mass / 200.f);

	planet.SetPosition(position);
	planet.SetMass(mass);

	// Get the nearest sun.
	auto nearestSun = Suns[0];

	for (auto& sun : Suns)
	{
		if ((sun - planet.Position()).Length() < (nearestSun - planet.Position()).Length())
		{
			nearestSun = sun;
		}
	}

	// Make the planet velocity perpendicular to the vector from the center of the screen to the planet.
	Vec2F centerToPlanet = nearestSun - planet.Position();
	// Calculate the velocity needed to make the planet orbit around the center of the screen using his mass.
	Vec2F orbitalVelocity = Vec2F(-centerToPlanet.Y, centerToPlanet.X).Normalized() * std::sqrt(SunMass * mass / centerToPlanet.Length());
	planet.SetVelocity(orbitalVelocity);
}

int main(int argc, char* args[])
{
	Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Planets");
	World::Init(1000);

	// Create sun
	Suns.emplace_back(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	// Create planets
	constexpr int planetsToCreate = PlanetsInteract ? 5 : 100;
	constexpr float R = 700;

	Planets.resize(planetsToCreate);

	for (std::size_t i = 0; i < planetsToCreate; i++)
	{
		auto randomAngle = Radian(Degree(Random::Range<float>(0.f, 360.f)));
		auto randomR = Random::Range<float>(R / 2.f, R);

		CreatePlanet(Vec2F(MathUtility::Cos(randomAngle), MathUtility::Sin(randomAngle)) * randomR + Suns[0]);
	}

    Display::Run();

    return 0;
}

void Update(float deltaTime)
{
	World::Update(deltaTime);

	if (Input::IsKeyHeld(SDL_SCANCODE_LSHIFT))
	{
		if (Input::IsMouseButtonPressed(SDL_BUTTON_LEFT))
		{
			CreatePlanet(Display::GetMousePosition(), Random::Range<float>(1000.f, 3000.f));
		}
	}
	else if (PlanetsInteract && Input::IsMouseButtonPressed(SDL_BUTTON_LEFT))
	{
		CreatePlanet(Display::GetMousePosition());
	}
	else if (!PlanetsInteract && Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
	{
		CreatePlanet(Display::GetMousePosition() + Vec2F(Random::Range<float>(-MouseRandomRadius, MouseRandomRadius), Random::Range<float>(-MouseRandomRadius, MouseRandomRadius)));
	}

	if (Input::IsMouseButtonPressed(SDL_BUTTON_MIDDLE))
	{
		// Create a sun at the mouse position.
		Suns.emplace_back(Display::GetMousePosition());
	}

	auto mouseWheelDelta = Input::GetMouseWheelDelta();

	if (mouseWheelDelta != 0)
	{
		Display::SetCameraZoom(Display::GetCameraZoom() + mouseWheelDelta * 0.01f);
	}

	auto mouseDelta = Display::GetMouseDelta();

	// Move camera when holding right mouse button.
	if (Input::IsMouseButtonHeld(SDL_BUTTON_RIGHT) && mouseDelta != Vec2F::Zero())
	{
		Display::MoveCamera(mouseDelta);
	}

	for (auto& planet : Planets)
	{
		auto& body = World::GetBody(planet.BodyRef);

		for (auto& sun : Suns)
		{
			// Apply force to planet to make it orbit around the center of the screen.
			Vec2F centerToPlanet = sun - body.Position();

			body.ApplyForce(centerToPlanet.Normalized() * (SunMass * body.Mass() / (centerToPlanet.Length() * centerToPlanet.Length())));
		}

		if (PlanetsInteract)
		{
			for (auto& otherPlanet: Planets)
			{
				auto& otherBody = World::GetBody(otherPlanet.BodyRef);

				if (&body == &otherBody)
				{ continue; }

				Vec2F planetToOtherPlanet = otherBody.Position() - body.Position();

				body.ApplyForce(planetToOtherPlanet.Normalized() * (otherBody.Mass() * body.Mass() / (planetToOtherPlanet.Length() * planetToOtherPlanet.Length())));
			}
		}

		Display::DrawCircle(body.Position().X, body.Position().Y, planet.Radius, planet.Color);
	}

	for (auto& sun : Suns)
	{
		Display::DrawCircle(sun.X, sun.Y, SunRadius, SunColor);
	}
}