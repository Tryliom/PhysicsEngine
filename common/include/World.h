#pragma once

class Body;

struct BodyRef
{
	size_t Index;
	size_t Generation;
};

namespace World
{
	void Init(size_t defaultBodySize = 500) noexcept;
	void Update(float deltaTime) noexcept;

	BodyRef CreateBody() noexcept;
	void DestroyBody(BodyRef bodyRef) noexcept;
	Body& GetBody(BodyRef bodyRef);
};