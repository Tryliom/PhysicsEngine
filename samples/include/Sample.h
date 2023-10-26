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
    /**
     * @brief Update the sample.
     * @param deltaTime The time since the last update.
     */
    void Update(float deltaTime) noexcept;
    /**
     * @brief Render the sample.
     */
    void Render() noexcept;

    /**
     * @brief Initialize the sample.
     */
    void Init() noexcept;
    /**
     * @brief Deinitialize the sample.
     */
    void Deinit() noexcept;

protected:
    /**
     * @brief Called when the sample is updated. This is where you should update your sample. The world is updated first.
     * @param deltaTime The time since the last update.
     */
    virtual void onUpdate(float deltaTime) noexcept = 0;
    /**
     * @brief Called when the sample is rendered. This is where you should render your sample. The display render all things after.
     */
    virtual void onRender() noexcept = 0;
    /**
     * @brief Called when the sample is initialized. This is where you should initialize your sample.
     */
    virtual void onInit() noexcept = 0;
    /**
     * @brief Called when the sample is deinitialized. This is where you should deinitialize your sample.
     */
    virtual void onDeinit() noexcept = 0;
};