#pragma once

#include "Vec2.h"

typedef union SDL_Event;

namespace Input
{
	void OnInput(SDL_Event event);

	void Update();

	/**
	 * @brief Returns true if the key was pressed this frame.
	 * @param key The key to check. Example: KB_KEY_A
	 * @return
	 */
	bool IsKeyPressed(int key);
	/**
	 * @brief Returns true if the key was released this frame.
	 * @param key The key to check. Example: KB_KEY_A
	 * @return
	 */
	bool IsKeyReleased(int key);
	/**
	 * @brief Returns true if the key is being held down.
	 * @param key The key to check. Example: KB_KEY_A
	 * @return
	 */
	bool IsKeyHeld(int key);

	/**
	 * @brief Returns true if the mouse button was pressed this frame.
	 * @param button The mouse button to check. Example: MOUSE_LEFT
	 * @return
	 */
	bool IsMouseButtonPressed(int button);
	/**
	 * @brief Returns true if the mouse button was released this frame.
	 * @param button The mouse button to check. Example: MOUSE_LEFT
	 * @return
	 */
	bool IsMouseButtonReleased(int button);
	/**
	 * @brief Returns true if the mouse button is being held down.
	 * @param button The mouse button to check. Example: MOUSE_LEFT
	 * @return
	 */
	bool IsMouseButtonHeld(int button);

	Vec2I GetMousePosition();
	Vec2I GetPreviousMousePosition();

	float GetMouseWheelDelta();
}