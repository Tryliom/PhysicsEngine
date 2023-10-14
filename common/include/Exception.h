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