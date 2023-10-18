#pragma once

#include <exception>

class SDLNotInitializedException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "SDL could not initialize!";
    }
};

class SDLWindowNotCreatedException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Window could not be created!";
    }
};

class SDLRendererNotCreatedException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Renderer could not be created!";
    }
};