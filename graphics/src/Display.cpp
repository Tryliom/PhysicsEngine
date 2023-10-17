#include "Display.h"
#include "Input.h"

#include "Timer.h"

#include "SDL.h"
#include <vector>

namespace Display
{
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	size_t _width = 0;
	size_t _height = 0;
	float _meterPerPixel = 1.f;

	Camera _camera;

	void Init(size_t width, size_t height, const std::string& name) noexcept
	{
		_width = width;
		_height = height;

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			return;
		}

		// Create window
		_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_SHOWN);

		// Enable VSync
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

		if (_window == nullptr)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return;
		}

		_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	}

	void Run() noexcept
	{
		Physics::Timer::Init();

		while (true)
		{
			Input::Update();

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT) return;

				Input::OnInput(e);
			}

			Physics::Timer::Update();

			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
			SDL_RenderClear(_renderer);

			Update(Physics::Timer::DeltaTime());

			SDL_RenderPresent(_renderer);
		}
	}

	void Shutdown() noexcept
	{
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
	}

	size_t GetWidth() noexcept
	{
		return _width;
	}

	size_t GetHeight() noexcept
	{
		return _height;
	}

	void SetMeterPerPixel(float meterPerPixel) noexcept
	{
		_meterPerPixel = meterPerPixel;
	}

	Math::Vec2F GetMousePosition() noexcept
	{
		return (Math::Vec2F{ Input::GetMousePosition() } - _camera.Position) / (_meterPerPixel * _camera.Zoom);
	}

	Math::Vec2F GetMouseDelta() noexcept
	{
		return Math::Vec2F{ Input::GetMouseDelta() };
	}

	Math::Vec2F GetCameraPosition() noexcept
	{
		return _camera.Position;
	}

	void MoveCamera(Math::Vec2F delta) noexcept
	{
		_camera.Position += delta;
	}

	void SetCameraPosition(Math::Vec2F position) noexcept
	{
		_camera.Position = position;
	}

	void LookAt(Math::Vec2F position) noexcept
	{
		const auto center = Math::Vec2F{ _width / 2.f, _height / 2.f };

		_camera.Position = center - position * _meterPerPixel * _camera.Zoom;
	}

	void SetCameraZoom(float zoom, Math::Vec2F targetPoint) noexcept
	{
		_camera.Position += targetPoint * _meterPerPixel * _camera.Zoom;

		_camera.Zoom = zoom;

		if (_camera.Zoom < 0.1f)
		{
			_camera.Zoom = 0.1f;
		}

		_camera.Position -= targetPoint * _meterPerPixel * _camera.Zoom;
	}

	float GetCameraZoom() noexcept
	{
		return _camera.Zoom;
	}

	void PushColor(Color color) noexcept
	{
		SDL_SetRenderDrawColor(_renderer, color.R, color.G, color.B, color.A);
	}

	void DrawCircle(float x, float y, float radius, Color color) noexcept
	{
		constexpr static int segments = 15;
		std::vector<SDL_Vertex> vertices;
		std::vector<int> indices;
		SDL_Color circleColor = { color.R, color.G, color.B, color.A };

		for (int i = 0; i < segments; i++)
		{
			auto angle = Math::Radian(Math::Degree(i * 360.f / segments));
			auto circleX = x + radius * Math::Utility::Cos(angle);
			auto circleY = y + radius * Math::Utility::Sin(angle);

			// Apply camera position and zoom
			circleX = _camera.Position.X + circleX * _meterPerPixel * _camera.Zoom;
			circleY = _camera.Position.Y + circleY * _meterPerPixel * _camera.Zoom;

			vertices.push_back({{ circleX, circleY}, circleColor, { 1.f, 1.f }});
		}

		for (int i = 0; i < segments - 1; i++)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		indices.push_back(0);
		indices.push_back(segments - 1);
		indices.push_back(0);

		SDL_RenderGeometry(_renderer, nullptr, vertices.data(), vertices.size(), indices.data(), indices.size());
	}
}