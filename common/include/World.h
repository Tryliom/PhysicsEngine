#pragma once

class Body;

struct BodyRef
{
	size_t Index;
	size_t Generation;

	constexpr bool operator==(const BodyRef& other) const noexcept
	{
		return Index == other.Index && Generation == other.Generation;
	}
};

namespace World
{
	void Init(size_t defaultBodySize = 500) noexcept;
	void Update(float deltaTime) noexcept;

	BodyRef CreateBody() noexcept;
	void DestroyBody(BodyRef bodyRef) noexcept;
	Body& GetBody(BodyRef bodyRef);
};