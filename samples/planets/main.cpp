#include "Display.h"

#include "Body.h"
#include "World.h"
#include "Vec2.h"
#include "Input.h"
#include "Random.h"

constexpr int SCREEN_WIDTH = 1550;
constexpr int SCREEN_HEIGHT = 900;

constexpr int PlanetRadius = 5;
constexpr float MouseRandomRadius = 15.f;
std::vector<std::pair<BodyRef, Color>> Planets;

constexpr float BaseVelocity = 400.f;
const float R = 700;

const Vec2F Sun = Vec2F(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
constexpr float SunMass = 100000.f;
constexpr int SunRadius = 3;

constexpr bool PlanetsInteract = false;

Color GenerateRandomColor()
{
	return Color(Random::Range<int>(0, 255), Random::Range<int>(0, 255), Random::Range<int>(0, 255));
}

void CreatePlanet(Vec2F position)
{
	auto planetRef = World::CreateBody();
	auto& planet = World::GetBody(planetRef);
	Planets.emplace_back(planetRef, GenerateRandomColor());

	planet.SetPosition(position);
	planet.SetMass(Random::Range<float>(900.f, 1100.f));

	// Make the planet velocity perpendicular to the vector from the center of the screen to the planet.
	Vec2F centerToPlanet = Sun - planet.Position();
	planet.SetVelocity(Vec2F(-centerToPlanet.Y, centerToPlanet.X).Normalized() * BaseVelocity);
}

int main(int argc, char* args[])
{
	Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Planets");
	World::Init(1000);

	// Create planets
	constexpr int planetsToCreate = 5;

	Planets.resize(planetsToCreate);

	for (std::size_t i = 0; i < planetsToCreate; i++)
	{
		auto randomAngle = Radian(Degree(Random::Range<float>(0.f, 360.f)));
		auto randomR = Random::Range<float>(R / 2.f, R);

		CreatePlanet(Vec2F(MathUtility::Cos(randomAngle), MathUtility::Sin(randomAngle)) * randomR + Sun);
	}

    Display::Run();

    return 0;
}

void Update(float deltaTime)
{
	World::Update(deltaTime);

	// If mouse clicked keep pressing, create a new planet.
	if (Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
	{
		CreatePlanet(Vec2F{ Input::GetMousePosition() } + Vec2F(Random::Range<float>(-MouseRandomRadius, MouseRandomRadius), Random::Range<float>(-MouseRandomRadius, MouseRandomRadius)));
	}

	for (auto& pair : Planets)
	{
		auto& planet = World::GetBody(pair.first);
		auto& color = pair.second;

		// Apply force to planet to make it orbit around the center of the screen.
		Vec2F centerToPlanet = Sun - planet.Position();

		planet.ApplyForce(centerToPlanet.Normalized() * (SunMass * planet.Mass() / (centerToPlanet.Length() * centerToPlanet.Length())));

		if (PlanetsInteract)
		{
			for (auto& otherPair: Planets)
			{
				auto& otherPlanet = World::GetBody(otherPair.first);

				if (&planet == &otherPlanet)
				{ continue; }

				Vec2F planetToOtherPlanet = otherPlanet.Position() - planet.Position();

				planet.ApplyForce(planetToOtherPlanet.Normalized() * (otherPlanet.Mass() * planet.Mass() / (planetToOtherPlanet.Length() * planetToOtherPlanet.Length())));
			}
		}

		if (centerToPlanet.Length() < Vec2F(SCREEN_WIDTH, SCREEN_HEIGHT).Length() / 2.f)
		{
			Display::PushColor(color);
			Display::DrawCircle(planet.Position().X, planet.Position().Y, PlanetRadius);
		}
	}

	Display::PushColor(Color(255, 255, 0));
	Display::DrawCircle(Sun.X, Sun.Y, SunRadius);
}