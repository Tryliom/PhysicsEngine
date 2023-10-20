#pragma once

#include <exception>

class InvalidBodyRefException : public std::exception
{
public:
	InvalidBodyRefException() noexcept = default;
	~InvalidBodyRefException() noexcept override = default;

	const char* what() const noexcept override
	{
		return "Invalid generation body reference";
	}
};

class InvalidColliderRefException : public std::exception
{
public:
	InvalidColliderRefException() noexcept = default;
	~InvalidColliderRefException() noexcept override = default;

	const char* what() const noexcept override
	{
		return "Invalid generation collider reference";
	}
};