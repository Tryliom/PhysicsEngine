#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>

/**
 * @brief Allocator interface
 */
class Allocator
{
protected:
    /**
     * @brief Root pointer to allocated memory
     */
    void* _rootPtr {};
    /**
     * @brief Current pointer to allocated memory
     */
    void* _currentPtr {};
    /**
     * @brief Size of allocated memory
     */
    std::size_t _size {};
    /**
     * @brief Number of allocations
     */
    std::size_t _allocations {};

public:
    Allocator() = default;
    Allocator(void* ptr, std::size_t size) noexcept;
    virtual ~Allocator() = default;

    /**
     * @brief Allocate memory
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] virtual void* Allocate(std::size_t size, std::size_t alignment) noexcept = 0;
    /**
     * @brief Deallocate memory
     * @param ptr Pointer to memory to deallocate
     */
    virtual void Deallocate(void* ptr) noexcept = 0;

    /**
     * @brief Get the root pointer
     */
    [[nodiscard]] void* GetRootPtr() const noexcept;
    /**
     * @brief Get the current pointer
     */
    [[nodiscard]] void* GetCurrentPtr() const noexcept;
    /**
     * @brief Get the size of allocated memory
     */
    [[nodiscard]] std::size_t GetSize() const noexcept;
    /**
     * @brief Get the number of allocations
     */
    [[nodiscard]] std::size_t GetAllocations() const noexcept;
};

/**
 * @brief Linear allocator
 */
class LinearAllocator final : public Allocator
{
private:
    std::size_t _offset = 0;

public:
    /**
     * @brief Constructor
     * @param size Size of memory to allocate
     */
    LinearAllocator(void* ptr, std::size_t size) noexcept;
    ~LinearAllocator() override;

    /**
     * @brief Allocate memory from allocator
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
    /**
     * @brief Does nothing for linear allocator
     */
    void Deallocate(void* ptr) noexcept override;

    /**
     * @brief Clear allocator
     */
    void Clear() noexcept;
};

/**
 * @brief Proxy allocator
 */
class ProxyAllocator final : public Allocator
{
private:
    Allocator& _allocator;

public:
    /**
     * @brief Constructor
     * @param allocator Allocator to proxy
     */
    explicit ProxyAllocator(Allocator& allocator) noexcept;
    ~ProxyAllocator() override = default;

    /**
     * @brief Allocate memory from allocator
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
    /**
     * @brief Deallocate memory from allocator
     * @param ptr Pointer to memory to deallocate
     */
    void Deallocate(void* ptr) noexcept override;
};

class HeapAllocator final : public Allocator
{
public:
	/**
	 * @brief Allocate memory from allocator
	 * @param size Size of memory to allocate
	 * @return Pointer to allocated memory
	 */
	[[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
	/**
	 * @brief Deallocate memory from allocator
	 * @param ptr Pointer to memory to deallocate
	 */
	void Deallocate(void* ptr) noexcept override;
};

template<typename T>
class StandardAllocator : public std::allocator<T>
{
protected:
	Allocator& _allocator;

public:
	typedef T value_type;
	explicit StandardAllocator(Allocator& allocator) noexcept : std::allocator<T>(), _allocator(allocator) {}
	template <class U>
	explicit StandardAllocator(const StandardAllocator<U>& allocator) noexcept : _allocator(allocator.GetAllocator()) {}

	T* allocate(std::size_t n);
	void deallocate(T* ptr, std::size_t n);

	[[nodiscard]] Allocator& GetAllocator() const { return _allocator; }
};

template<typename T>
using AllocVector = std::vector<T, StandardAllocator<T>>;