#include "Display.h"
#include "Input.h"
#include "Exception.h"

#include "SDL.h"
#include "Mat4x4.h"

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

	void Draw(Math::CircleF circle, Color color, Math::Vec2F scale, int segments) noexcept
	{
        if (!IsVisible(circle)) return;

		SDL_Color circleColor = { color.R, color.G, color.B, color.A };
        const int offset = static_cast<int>(_vertices.size());
		const auto x = circle.Center().X;
		const auto y = circle.Center().Y;
		const auto radius = circle.Radius() * scale.X;

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

	void Draw(Math::RectangleF rectangle, Color color, Math::Vec2F scale) noexcept
	{
		if (!IsVisible(rectangle)) return;

		SDL_Color rectangleColor = { color.R, color.G, color.B, color.A };
		const auto center = rectangle.Center();
		auto scaledRectangle = rectangle;

		scaledRectangle.SetMinBound(center + (rectangle.MinBound() - center) * scale);
		scaledRectangle.SetMaxBound(center + (rectangle.MaxBound() - center) * scale);

		const auto x = scaledRectangle.MinBound().X;
		const auto y = scaledRectangle.MinBound().Y;
		const auto width = Math::Abs(scaledRectangle.MaxBound().X - scaledRectangle.MinBound().X);
		const auto height = Math::Abs(scaledRectangle.MaxBound().Y - scaledRectangle.MinBound().Y);

		// Apply camera position and zoom
		const auto rectangleX = _camera.Position.X + x * _meterPerPixel * _camera.Zoom;
		const auto rectangleY = _camera.Position.Y + y * _meterPerPixel * _camera.Zoom;
		const auto rectangleWidth = width * _meterPerPixel * _camera.Zoom;
		const auto rectangleHeight = height * _meterPerPixel * _camera.Zoom;

		_vertices.push_back({{ rectangleX, rectangleY }, rectangleColor, { 1.f, 1.f }});
		_vertices.push_back({{ rectangleX + rectangleWidth, rectangleY }, rectangleColor, { 1.f, 1.f }});
		_vertices.push_back({{ rectangleX + rectangleWidth, rectangleY + rectangleHeight }, rectangleColor, { 1.f, 1.f }});
		_vertices.push_back({{ rectangleX, rectangleY + rectangleHeight }, rectangleColor, { 1.f, 1.f }});

		_indices.push_back(static_cast<int>(_vertices.size()) - 4);
		_indices.push_back(static_cast<int>(_vertices.size()) - 3);
		_indices.push_back(static_cast<int>(_vertices.size()) - 2);
		_indices.push_back(static_cast<int>(_vertices.size()) - 4);
		_indices.push_back(static_cast<int>(_vertices.size()) - 2);
		_indices.push_back(static_cast<int>(_vertices.size()) - 1);
	}

	void Draw(const Math::PolygonF& polygon, Color color, Math::Vec2F scale) noexcept
	{
		if (!IsVisible(polygon)) return;

		SDL_Color polygonColor = { color.R, color.G, color.B, color.A };
		const auto& vertices = polygon.Vertices();
		const auto center = polygon.Center();

		for (size_t i = 0; i < vertices.size(); i++)
		{
			const auto vertex = center + (vertices[i] - center) * scale;
			const auto nextVertex = center + (vertices[(i + 1) % vertices.size()] - center) * scale;

			const auto vertexX = _camera.Position.X + vertex.X * _meterPerPixel * _camera.Zoom;
			const auto vertexY = _camera.Position.Y + vertex.Y * _meterPerPixel * _camera.Zoom;
			const auto nextVertexX = _camera.Position.X + nextVertex.X * _meterPerPixel * _camera.Zoom;
			const auto nextVertexY = _camera.Position.Y + nextVertex.Y * _meterPerPixel * _camera.Zoom;

			_vertices.push_back({{ vertexX, vertexY }, polygonColor, { 1.f, 1.f }});
			_vertices.push_back({{ nextVertexX, nextVertexY }, polygonColor, { 1.f, 1.f }});

			if (i > 0)
			{
				_indices.push_back(static_cast<int>(_vertices.size()) - 4);
				_indices.push_back(static_cast<int>(_vertices.size()) - 3);
				_indices.push_back(static_cast<int>(_vertices.size()) - 2);
				_indices.push_back(static_cast<int>(_vertices.size()) - 4);
				_indices.push_back(static_cast<int>(_vertices.size()) - 2);
				_indices.push_back(static_cast<int>(_vertices.size()) - 1);
			}
		}
	}

    bool IsVisible(Math::CircleF circle) noexcept
    {
	    const float radius = circle.Radius();
	    const Math::Vec2F positionRelativeToCamera = _camera.Position + circle.Center() * _meterPerPixel * _camera.Zoom;

	    const float left = positionRelativeToCamera.X - radius;
	    const float right = positionRelativeToCamera.X + radius;
	    const float top = positionRelativeToCamera.Y - radius;
	    const float bottom = positionRelativeToCamera.Y + radius;

	    return left < static_cast<float>(_width) && right > 0 && top < static_cast<float>(_height) && bottom > 0;
    }

	bool IsVisible(Math::RectangleF rectangle) noexcept
	{
		const Math::Vec2F minBound = rectangle.MinBound();
		const Math::Vec2F maxBound = rectangle.MaxBound();

		const Math::Vec2F minBoundRelativeToCamera = _camera.Position + minBound * _meterPerPixel * _camera.Zoom;
		const Math::Vec2F maxBoundRelativeToCamera = _camera.Position + maxBound * _meterPerPixel * _camera.Zoom;

		const float left = minBoundRelativeToCamera.X;
		const float right = maxBoundRelativeToCamera.X;
		const float top = minBoundRelativeToCamera.Y;
		const float bottom = maxBoundRelativeToCamera.Y;

		return left < static_cast<float>(_width) && right > 0 && top < static_cast<float>(_height) && bottom > 0;
	}

	bool IsVisible(const Math::PolygonF& polygon) noexcept
	{
		const auto& vertices = polygon.Vertices();

		for (size_t i = 0; i < vertices.size(); i++)
		{
			const auto& vertex = vertices[i];
			const auto& nextVertex = vertices[(i + 1) % vertices.size()];

			const Math::Vec2F vertexRelativeToCamera = _camera.Position + vertex * _meterPerPixel * _camera.Zoom;
			const Math::Vec2F nextVertexRelativeToCamera = _camera.Position + nextVertex * _meterPerPixel * _camera.Zoom;

			const float left = vertexRelativeToCamera.X;
			const float right = nextVertexRelativeToCamera.X;
			const float top = vertexRelativeToCamera.Y;
			const float bottom = nextVertexRelativeToCamera.Y;

			if (left < static_cast<float>(_width) && right > 0 && top < static_cast<float>(_height) && bottom > 0)
			{
				return true;
			}
		}

		return false;
	}
}