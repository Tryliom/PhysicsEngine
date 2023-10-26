#pragma once

#include "Vec2.h"
#include "Shape.h"
#include "Color.h"

#include <string>
#include <SDL_events.h>

/**
 * @brief Camera struct. Contains the position and the zoom of the camera.
 */
struct Camera
{
	Math::Vec2F Position { 0.f, 0.f };
	float Zoom { 1.f };
};

/**
 * @brief Display namespace. Contains all functions to create and draw in a window.
 */
namespace Display
{
    /**
     * @brief Initialize the display, create a new window and a new renderer.
     * @param width Width of the display
     * @param height Height of the display
     * @param name Name of the display
     */
	void Init(size_t width, size_t height, const std::string& name = "Display");
    /**
     * @brief Draw all vertices on the screen.
     */
	void Render() noexcept;
    /**
     * @brief Close the display, destroy the window and the renderer.
     */
	void Shutdown() noexcept;
    /**
     * @brief Reset the camera, zoom and meterPerPixel to the default position.
     */
    void ResetView() noexcept;

    /**
     * @brief Resize the screen.
     * @param width Width of the screen
     * @param height Height of the screen
     */
    void Resize(size_t width, size_t height) noexcept;
    /**
     * @brief Set the title of the window.
     * @param title
     */
    void SetTitle(const std::string& title) noexcept;

    /**
     * @brief Get the width of the display.
     * @return Width of the display
     */
	size_t GetWidth() noexcept;
    /**
     * @brief Get the height of the display.
     * @return Height of the display
     */
	size_t GetHeight() noexcept;

    /**
     * @brief Set the meter per pixel ratio.
     * @param meterPerPixel Meter per pixel ratio
     */
	void SetMeterPerPixel(float meterPerPixel) noexcept;
    /**
     * @brief Get the meter per pixel ratio.
     * @return Meter per pixel ratio
     */
	Math::Vec2F GetMousePosition() noexcept;
    /**
     * @brief Get the mouse position.
     * @return Mouse position
     */
	Math::Vec2F GetMouseDelta() noexcept;
    /**
     * @brief Get the mouse wheel delta.
     * @return Mouse wheel delta
     */
	Math::Vec2F GetCameraPosition() noexcept;

    /**
     * @brief Move the camera.
     * @param delta The amount of movement using to move the camera
     */
	void MoveCamera(Math::Vec2F delta) noexcept;
    /**
     * @brief Set the camera position.
     * @param position The position of the camera
     */
	void SetCameraPosition(Math::Vec2F position) noexcept;
    /**
     * @brief Look at the position with the camera.
     * @param position The position to look at
     */
	void LookAt(Math::Vec2F position) noexcept;
    /**
     * @brief Set the camera zoom.
     * @param zoom The zoom of the camera
     * @param targetPoint The point to zoom at
     */
	void SetCameraZoom(float zoom, Math::Vec2F targetPoint) noexcept;
    /**
     * @brief Get the camera zoom.
     * @return The zoom of the camera
     */
	float GetCameraZoom() noexcept;

    /**
     * @brief Clear the screen with a color. Clear the vertices and indices.
     */
    void ClearRender() noexcept;

    /**
     * @brief Draw a circle.
     * @param circle The circle to draw
     * @param color The color of the circle
     * @param scale The scale of the circle
     * @param segments The number of segments of the circle. The higher the number, the more precise the circle will be
     */
	void Draw(Math::CircleF circle, Color color, Math::Vec2F scale = Math::Vec2F::One(), int segments = 15) noexcept;
    /**
     * @brief Draw a rectangle.
     * @param rectangle The rectangle to draw
     * @param color The color of the rectangle
     * @param scale The scale of the rectangle
     */
	void Draw(Math::RectangleF rectangle, Color color, Math::Vec2F scale = Math::Vec2F::One()) noexcept;
    /**
     * @brief Draw a polygon.
     * @param polygon The polygon to draw
     * @param color The color of the polygon
     * @param scale The scale of the polygon
     */
	void Draw(const Math::PolygonF& polygon, Color color, Math::Vec2F scale = Math::Vec2F::One()) noexcept;

    /**
     * @brief Check if the circle is visible on the screen (with the camera).
     * @param circle The circle to check
     * @return True if the circle is visible, false otherwise
     */
    bool IsVisible(Math::CircleF circle) noexcept;
    /**
     * @brief Check if the rectangle is visible on the screen (with the camera).
     * @param rectangle The rectangle to check
     * @return True if the rectangle is visible, false otherwise
     */
	bool IsVisible(Math::RectangleF rectangle) noexcept;
    /**
     * @brief Check if the polygon is visible on the screen (with the camera).
     * @param polygon The polygon to check
     * @return True if the polygon is visible, false otherwise
     */
	bool IsVisible(const Math::PolygonF& polygon) noexcept;
}