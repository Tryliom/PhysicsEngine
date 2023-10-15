#include "Input.h"

#include <SDL.h>

namespace Input
{
	bool _keysPressed[512];
	bool _keysReleased[512];
	bool _keysHeld[512];

	bool _mouseButtonsPressed[5];
	bool _mouseButtonsReleased[5];
	bool _mouseButtonsHeld[5];

	Vec2I _mousePosition;
	Vec2I _previousMousePosition;

	float _mouseWheelDelta;

	void OnInput(SDL_Event event)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			_keysPressed[event.key.keysym.scancode] = true;
			_keysHeld[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			_keysReleased[event.key.keysym.scancode] = true;
			_keysHeld[event.key.keysym.scancode] = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mouseButtonsPressed[event.button.button] = true;
			_mouseButtonsHeld[event.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			_mouseButtonsReleased[event.button.button] = true;
			_mouseButtonsHeld[event.button.button] = false;
			break;
		case SDL_MOUSEWHEEL:
			_mouseWheelDelta = event.wheel.y;
			break;
		case SDL_MOUSEMOTION:
			_previousMousePosition = _mousePosition;
			_mousePosition.X = event.motion.x;
			_mousePosition.Y = event.motion.y;
			break;
		}
	}

	void Update()
	{
		for (int i = 0; i < 512; i++)
		{
			_keysPressed[i] = false;
			_keysReleased[i] = false;
		}

		for (int i = 0; i < 5; i++)
		{
			_mouseButtonsPressed[i] = false;
			_mouseButtonsReleased[i] = false;
		}

		_mouseWheelDelta = 0;
	}

	bool IsKeyPressed(int key)
	{
		return _keysPressed[key];
	}

	bool IsKeyReleased(int key)
	{
		return _keysReleased[key];
	}

	bool IsKeyHeld(int key)
	{
		return _keysHeld[key];
	}

	bool IsMouseButtonPressed(int button)
	{
		return _mouseButtonsPressed[button];
	}

	bool IsMouseButtonReleased(int button)
	{
		return _mouseButtonsReleased[button];
	}

	bool IsMouseButtonHeld(int button)
	{
		return _mouseButtonsHeld[button];
	}

	Vec2I GetMousePosition()
	{
		return _mousePosition;
	}

	Vec2I GetPreviousMousePosition()
	{
		return _previousMousePosition;
	}

	float GetMouseWheelDelta()
	{
		return _mouseWheelDelta;
	}
}