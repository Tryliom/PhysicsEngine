#include "Display.h"
#include "Input.h"
#include "Exception.h"

#include "SDL.h"

#include <vector>

namespace Display
{
    static SDL_Window* _window = nullptr;
    static SDL_Renderer* _renderer = nullptr;

    static std::vector<SDL_Vertex> _vertices;
    static std::vector<int> _indices;

    static std::size_t _width = 0;
    static std::size_t _height = 0;
    static float _meterPerPixel = 1.f;

    static Camera _camera;

	void Init(size_t width, size_t height, const std::string& name)
	{
		_width = width;
		_height = height;

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			throw SDLNotInitializedException();
		}

		// Create window
		_window = SDL_CreateWindow(
            name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            static_cast<int>(_width),
            static_cast<int>(_height),
            SDL_WINDOW_SHOWN
        );


		if (_window == nullptr)
		{
            throw SDLWindowNotCreatedException();
		}

        // Enable VSync
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

		_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

        if (_renderer == nullptr)
        {
            throw SDLRendererNotCreatedException();
        }

        ClearRender();
	}

	void Render() noexcept
	{
        SDL_RenderGeometry(
            _renderer,
            nullptr,
            _vertices.data(),
            static_cast<int>(_vertices.size()),
            _indices.data(),
            static_cast<int>(_indices.size())
        );
        SDL_RenderPresent(_renderer);
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
		const auto center = Math::Vec2F{ static_cast<float>(_width) / 2.f, static_cast<float>(_height) / 2.f };

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

    void ClearRender() noexcept
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);

        _vertices.clear();
        _indices.clear();
    }

	void DrawCircle(float x, float y, float radius, Color color, int segments) noexcept
	{
        if (!IsVisible({ x, y }, radius)) return;

		SDL_Color circleColor = { color.R, color.G, color.B, color.A };
        const int offset = static_cast<int>(_vertices.size());

		for (int i = 0; i < segments; i++)
		{
			auto angle = Math::Radian(Math::Degree(static_cast<float>(i) * 360.f / static_cast<float>(segments)));
			auto circleX = x + radius * Math::Cos(angle);
			auto circleY = y + radius * Math::Sin(angle);

			// Apply camera position and zoom
			circleX = _camera.Position.X + circleX * _meterPerPixel * _camera.Zoom;
			circleY = _camera.Position.Y + circleY * _meterPerPixel * _camera.Zoom;

			_vertices.push_back({{ circleX, circleY}, circleColor, { 1.f, 1.f }});

            if (i > 0)
            {
                _indices.push_back(offset);
                _indices.push_back(offset + i - 1);
                _indices.push_back(offset + i);
            }
		}

        _indices.push_back(offset);
        _indices.push_back(offset + segments - 1);
        _indices.push_back(offset + 1);
	}

    bool IsVisible(Math::Vec2F position, float radius) noexcept
    {
        const Math::Vec2F positionRelativeToCamera = _camera.Position + position * _meterPerPixel * _camera.Zoom;

        const float left = positionRelativeToCamera.X - radius;
        const float right = positionRelativeToCamera.X + radius;
        const float top = positionRelativeToCamera.Y - radius;
        const float bottom = positionRelativeToCamera.Y + radius;

        return left < static_cast<float>(_width) && right > 0 && top < static_cast<float>(_height) && bottom > 0;
    }
}