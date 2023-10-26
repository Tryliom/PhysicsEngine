#pragma once

#include "Vec2.h"

typedef union SDL_Event SDL_Event;

namespace Input
{
    /**
     * @brief Called when an input event occurs.
     * @param event The input event.
     */
	void OnInput(SDL_Event event);

    /**
     * @brief Updates the input state.
     */
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

    /**
     * @brief Returns the mouse position.
     * @return
     */
	Math::Vec2I GetMousePosition();
    /**
     * @brief Returns the mouse delta.
     * @return
     */
	Math::Vec2I GetMouseDelta();

    /**
     * @brief Returns the mouse wheel delta.
     * @return
     */
	float GetMouseWheelDelta();
}