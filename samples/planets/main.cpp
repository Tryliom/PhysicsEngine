#include "Display.h"

#include "Body.h"
#include "World.h"
#include "Random.h"
#include "Vec2.h"

#include "SDL.h"

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

SDL_Renderer* gRenderer = nullptr;

constexpr int SCREEN_WIDTH = 1550;
constexpr int SCREEN_HEIGHT = 900;

int PlanetRadius = 5;
std::vector<Body> Planets;

Vec2F Center = Vec2F(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
float AttractionPower = 900.f;
float BaseVelocity = 400.f;
float MaxVelocity = 700.f;
const float R = 200;

int main(int argc, char* args[])
{
	Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Planets");
	World::Init(10000);

	// Create planets
	constexpr int planetsToCreate = 1000;

	Planets.resize(planetsToCreate);

	//TODO: Use bodies instead of planets

	for (std::size_t i = 0; i < planetsToCreate; i++)
	{
		auto randomAngle = Radian(Degree(Random::Range<float>(0.f, 360.f)));
		auto randomR = Random::Range<float>(R / 2.f, R);

		Planets[i].Position = Vec2F(MathUtility::Cos(randomAngle), MathUtility::Sin(randomAngle)) * randomR + Center;
		Planets[i].Acceleration = Vec2F(0, 0);

		// Make the planet velocity perpendicular to the vector from the center of the screen to the planet.
		Vec2F centerToPlanet = Center - Planets[i].Position;
		Planets[i].Velocity = Vec2F(-centerToPlanet.Y, centerToPlanet.X).Normalized() * BaseVelocity;
	}

    Display::Run();

    return 0;
}

void Update(float deltaTime)
{
	for (auto& planet : Planets)
	{
		// Calculate the vector from the center of the screen to the planet.
		Vec2F centerToPlanet = Center - planet.Position;

		// Make them orbit around the center of the screen.
		planet.Acceleration = centerToPlanet.Normalized() * AttractionPower;
	}

	for (const auto& planet : Planets)
	{
		SDL_SetRenderDrawColor(gRenderer, 200, 100, 100, 255);

		Display::DrawCircle(planet.Position.X, planet.Position.Y, PlanetRadius);
	}
}
