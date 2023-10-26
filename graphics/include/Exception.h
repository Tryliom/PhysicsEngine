#pragma once

#include <exception>

/**
 * @brief Exception thrown when SDL could not initialize.
 */
class SDLNotInitializedException : public std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "SDL could not initialize!";
    }
};

/**
 * @brief Exception thrown when SDL could not create a window.
 */
class SDLWindowNotCreatedException : public std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "Window could not be created!";
    }
};

/**
 * @brief Exception thrown when SDL could not create a renderer.
 */
class SDLRendererNotCreatedException : public std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "Renderer could not be created!";
    }
};