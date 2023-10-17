#pragma once

namespace Physics::Timer
{
	void Init() noexcept;

	void Update() noexcept;

	/**
	 * @brief Get the Delta Time in seconds
	 */
	float DeltaTime() noexcept;
}