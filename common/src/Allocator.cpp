#include "../include/Allocator.h"

#include <cstdlib>

Allocator::Allocator(void* ptr, std::size_t size) noexcept :
    _rootPtr(ptr), _currentPtr(ptr), _size(size), _allocations(0) {}

void* Allocator::GetRootPtr() const noexcept
{
    return _rootPtr;
}

void* Allocator::GetCurrentPtr() const noexcept
{
    return _currentPtr;
}

std::size_t Allocator::GetSize() const noexcept
{
    return _size;
}

std::size_t Allocator::GetAllocations() const noexcept
{
    return _allocations;
}

// LinearAllocator implementation

LinearAllocator::LinearAllocator(void* ptr, std::size_t size) noexcept :
    Allocator(ptr, size), _offset(0) {}

LinearAllocator::~LinearAllocator()
{
    std::free(_rootPtr);
}

void* LinearAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
    // Align the current pointer
    std::size_t padding = 0;
    std::size_t alignedAddress = reinterpret_cast<std::size_t>(_currentPtr) + _offset;

    if (alignment != 0 && alignedAddress % alignment != 0)
    {
        padding = alignment - (alignedAddress % alignment);
    }

    // Check if there is enough memory
    if (_offset + padding + size > _size)
    {
        return nullptr;
    }

    // Update the current pointer
    _offset += padding;
    void* alignedPtr = reinterpret_cast<void*>(alignedAddress + padding);
    _offset += size;
    _currentPtr = alignedPtr;

    // Update the number of allocations
    _allocations++;

    return alignedPtr;
}

void LinearAllocator::Deallocate(void* ptr) noexcept
{
    // Do nothing
}

void LinearAllocator::Clear() noexcept
{
    _offset = 0;
    _currentPtr = _rootPtr;
    _allocations = 0;
}

// ProxyAllocator implementation

ProxyAllocator::ProxyAllocator(Allocator& allocator) noexcept :
    Allocator(allocator), _allocator(allocator) {}

void* ProxyAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
    auto ptr = _allocator.Allocate(size, alignment);

    // Update the number of allocations
    if (ptr != nullptr)
    {
        _allocations++;
    }

    return ptr;
}

void ProxyAllocator::Deallocate(void* ptr) noexcept
{
    _allocator.Deallocate(ptr);
}