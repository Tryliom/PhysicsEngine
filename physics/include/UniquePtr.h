#pragma once

#include <algorithm>

//TODO: Add MakeUnique

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

		constexpr UniquePtr(UniquePtr&& other) noexcept
        {
            std::swap(_ptr, other._ptr);
        }

		constexpr UniquePtr& operator=(UniquePtr&& other) noexcept
        {
            std::swap(_ptr, other._ptr);

            return *this;
        }

		constexpr UniquePtr(const UniquePtr& other) noexcept = delete;
		constexpr UniquePtr& operator=(const UniquePtr& other) noexcept = delete;

		constexpr ~UniquePtr() noexcept
		{
            delete _ptr;
		}

	private:
		T* _ptr = nullptr;

	public:
		UniquePtr() noexcept = delete;

        [[nodiscard]] constexpr T* operator->() noexcept
        {
            return _ptr;
        }

        [[nodiscard]] constexpr const T* operator->() const noexcept
        {
            return _ptr;
        }

        [[nodiscard]] constexpr T& operator*() noexcept
        {
            return *_ptr;
        }

        [[nodiscard]] constexpr const T& operator*() const noexcept
        {
            return *_ptr;
        }

		constexpr T* Get() noexcept
		{
			return _ptr;
		}
	};

    template <typename T>
    UniquePtr<T> MakeUnique(T value) noexcept
    {
        return UniquePtr<T>(new T(value));
    }
}