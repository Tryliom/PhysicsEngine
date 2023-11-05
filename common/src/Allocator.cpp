#include <cassert>
#include "Allocator.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

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

std::size_t Allocator::calculateAlignForwardAdjustment(const void* address, std::size_t alignment)
{
	assert((alignment & (alignment - 1)) == 0 && "Alignement needs to be a power of two");
	const std::size_t adjustment = alignment - (reinterpret_cast<std::uintptr_t>(address) & (alignment - 1));

	if (adjustment == alignment) return 0;
	return adjustment;
}

std::size_t Allocator::calculateAlignForwardAdjustmentWithHeader(const void* address, std::size_t alignment,
                                                                       std::size_t headerSize)
{
	auto adjustment = calculateAlignForwardAdjustment(address, alignment);
	std::size_t neededSpace = headerSize;

	if (adjustment < neededSpace)
	{
		neededSpace -= adjustment;
		adjustment += alignment * (neededSpace / alignment);

		if (neededSpace % alignment > 0) adjustment += alignment;
	}

	return adjustment;
}

void* Allocator::alignForward(void* address, std::size_t alignment)
{
	return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(address) + calculateAlignForwardAdjustment(address, alignment));
}

void* Allocator::alignForwardWithHeader(void* address, std::size_t alignment, std::size_t headerSize)
{
	return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(address) + calculateAlignForwardAdjustmentWithHeader(address, alignment, headerSize));
}

// LinearAllocator implementation

LinearAllocator::LinearAllocator(void* ptr, std::size_t size) noexcept :
    Allocator(ptr, size), _offset(0) {}

LinearAllocator::~LinearAllocator()
{
    std::free(_rootPtr);
}

void LinearAllocator::Init(void* ptr, std::size_t size) noexcept
{
	if (_rootPtr != nullptr)
	{
		std::free(_rootPtr);
	}

	_rootPtr = ptr;
	_currentPtr = ptr;
	_size = size;
	_offset = 0;
	_allocations = 0;
}

void* LinearAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
	assert(size != 0 && "Linear Allocator cannot allocated nothing");
	const auto adjustment = calculateAlignForwardAdjustment(_currentPtr, alignment);

	assert(_offset + adjustment + size < _size && "Linear Allocator has not enough space for this allocation");

	auto* alignedAddress = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(_currentPtr) + adjustment);
	_currentPtr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(alignedAddress) + size);
	_offset += size + adjustment;
	_allocations++;

	return alignedAddress;
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

#ifdef TRACY_ENABLE
	TracyAlloc(ptr, size * alignment);
#endif

    // Update the number of allocations
    if (ptr != nullptr)
    {
        _allocations++;
    }

    return ptr;
}

void ProxyAllocator::Deallocate(void* ptr) noexcept
{
#ifdef TRACY_ENABLE
	TracyFree(ptr);
#endif

    _allocator.Deallocate(ptr);
}

void* HeapAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
	if (size == 0) return nullptr;

	const auto space = size * alignment;
	auto* ptr = std::malloc(space);

#ifdef TRACY_ENABLE
	TracyAlloc(ptr, space);
#endif

	return ptr;
}

void HeapAllocator::Deallocate(void* ptr) noexcept
{
	if (ptr == nullptr) return;

#ifdef TRACY_ENABLE
	TracyFree(ptr);
#endif

	std::free(ptr);
}