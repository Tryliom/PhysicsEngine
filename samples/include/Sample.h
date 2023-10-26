#pragma once

#include "World.h"

class Sample
{
public:
    Sample() noexcept = default;
    virtual ~Sample() noexcept = default;

protected:
    Physics::World _world;

public:
    void Update(float deltaTime) noexcept;
    void Render() noexcept;

    void Init() noexcept;
    void Deinit() noexcept;

protected:
    virtual void onUpdate(float deltaTime) noexcept = 0;
    virtual void onRender() noexcept = 0;
    virtual void onInit() noexcept = 0;
    virtual void onDeinit() noexcept = 0;
};