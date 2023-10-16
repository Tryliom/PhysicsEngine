#include "Display.h"
#include "Input.h"

#include "Timer.h"

#include "SDL.h"

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
		Timer::Init();

		while (true)
		{
			Input::Update();

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT) return;

				Input::OnInput(e);
			}

			Timer::Update();

			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
			SDL_RenderClear(_renderer);

			Update(Timer::DeltaTime());

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

	Vec2F GetMousePosition() noexcept
	{
		return (Vec2F{ Input::GetMousePosition() } - _camera.Position) / (_meterPerPixel * _camera.Zoom);
	}

	Vec2F GetMouseDelta() noexcept
	{
		return Vec2F{ Input::GetMouseDelta() };
	}

	void MoveCamera(Vec2F delta) noexcept
	{
		_camera.Position += delta;
	}

	void SetCameraPosition(Vec2F position) noexcept
	{
		_camera.Position = position;
	}

	void SetCameraZoom(float zoom) noexcept
	{
		// Zoom in/out around the mouse position
		auto mousePosition = GetMousePosition();

		_camera.Position += mousePosition * _meterPerPixel * _camera.Zoom;

		_camera.Zoom = zoom;

		if (_camera.Zoom < 0.1f)
		{
			_camera.Zoom = 0.1f;
		}

		_camera.Position -= mousePosition * _meterPerPixel * _camera.Zoom;
	}

	float GetCameraZoom() noexcept
	{
		return _camera.Zoom;
	}

	void PushColor(Color color) noexcept
	{
		SDL_SetRenderDrawColor(_renderer, color.R, color.G, color.B, color.A);
	}

	void DrawCircle(int x, int y, int radius) noexcept
	{
		for (int w = 0; w < radius * 2; w++)
		{
			for (int h = 0; h < radius * 2; h++)
			{
				int dx = radius - w; // horizontal offset
				int dy = radius - h; // vertical offset

				if ((dx*dx + dy*dy) <= (radius * radius))
				{
					// Cancel draw if outside of screen taking camera position and zoom into account.
					if (x - radius + w < -_camera.Position.X / (_meterPerPixel * _camera.Zoom) || x - radius + w > -_camera.Position.X / (_meterPerPixel * _camera.Zoom) + _width / (_meterPerPixel * _camera.Zoom) ||
						y - radius + h < -_camera.Position.Y / (_meterPerPixel * _camera.Zoom) || y - radius + h > -_camera.Position.Y / (_meterPerPixel * _camera.Zoom) + _height / (_meterPerPixel * _camera.Zoom))
					{
						continue;
					}

					SDL_RenderDrawPoint(_renderer, _camera.Position.X + (x - radius + w) * _meterPerPixel * _camera.Zoom, _camera.Position.Y + (y - radius + h) * _meterPerPixel * _camera.Zoom);
				}
			}
		}
	}
}