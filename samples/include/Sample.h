#pragma once

#include "World.h"

#include <string>

class Sample
{
public:
    Sample(std::string name, std::string description) noexcept;
    virtual ~Sample() noexcept = default;

private:
	std::string _name;
	std::string _description;

protected:
    Physics::World _world;

public:
	[[nodiscard]] constexpr std::string GetName() const noexcept { return _name; }
	[[nodiscard]] constexpr std::string GetDescription() const noexcept { return _description; }

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
    /**
     * @brief Draw the ImGui window for the sample.
     */
    void DrawImGui() noexcept;

protected:
	/**
	 * @brief Called before onUpdate. This is where you should handle mouse and keyboard inputs. This will be ignored if the ImGui context is hovered.
	 */
	virtual void onInput() noexcept = 0;
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
    /**
     * @brief Called when imGui window is drew, place your options for the sample here.
     */
    virtual void onDrawImGui() noexcept = 0;
};