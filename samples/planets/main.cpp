#include "Display.h"

#include "Body.h"
#include "World.h"
#include "Vec2.h"
#include "Input.h"
#include "Random.h"

constexpr int SCREEN_WIDTH = 1550;
constexpr int SCREEN_HEIGHT = 900;

int PlanetRadius = 5;
float MouseRandomRadius = 15.f;
std::vector<std::pair<BodyRef, Color>> Planets;

Vec2F Center = Vec2F(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
float AttractionPower = 900.f;
float BaseVelocity = 400.f;
const float R = 400;

Color GenerateRandomColor()
{
	return Color(Random::Range<int>(0, 255), Random::Range<int>(0, 255), Random::Range<int>(0, 255));
}

void CreatePlanet(Vec2F position)
{
	auto planetRef = World::CreateBody();
	auto& planet = World::GetBody(planetRef);
	Planets.emplace_back(planetRef, GenerateRandomColor());

	planet.Position = position;
	planet.Acceleration = Vec2F(0, 0);

	// Make the planet velocity perpendicular to the vector from the center of the screen to the planet.
	Vec2F centerToPlanet = Center - planet.Position;
	planet.Velocity = Vec2F(-centerToPlanet.Y, centerToPlanet.X).Normalized() * BaseVelocity;
}

int main(int argc, char* args[])
{
	Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Planets");
	World::Init(1000);

	// Create planets
	constexpr int planetsToCreate = 1000;

	Planets.resize(planetsToCreate);

	for (std::size_t i = 0; i < planetsToCreate; i++)
	{
		auto randomAngle = Radian(Degree(Random::Range<float>(0.f, 360.f)));
		auto randomR = Random::Range<float>(R / 2.f, R);

		CreatePlanet(Vec2F(MathUtility::Cos(randomAngle), MathUtility::Sin(randomAngle)) * randomR + Center);
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

		// Calculate the vector from the center of the screen to the planet.
		Vec2F centerToPlanet = Center - planet.Position;

		// Make them orbit around the center of the screen.
		planet.Acceleration = centerToPlanet.Normalized() * AttractionPower;

		Display::PushColor(color);
		Display::DrawCircle(planet.Position.X, planet.Position.Y, PlanetRadius);
	}
}