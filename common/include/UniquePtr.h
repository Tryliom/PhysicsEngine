#pragma once

namespace Physics
{
	template<typename T>
	class UniquePtr
	{
	public:
		constexpr explicit UniquePtr(T* ptr) noexcept
		{
			_ptr = ptr;
		}

		constexpr UniquePtr(UniquePtr&& other) noexcept = delete;
		constexpr UniquePtr(const UniquePtr& other) noexcept = delete;
		constexpr UniquePtr& operator=(UniquePtr&& other) noexcept = delete;
		constexpr UniquePtr& operator=(const UniquePtr& other) noexcept = delete;

		constexpr ~UniquePtr() noexcept
		{
			delete _ptr;
		}

	private:
		T* _ptr = nullptr;

	public:
		UniquePtr() noexcept = delete;

		constexpr T* Get() noexcept
		{
			return _ptr;
		}
	};
}