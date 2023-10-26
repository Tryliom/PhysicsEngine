#include "Allocator.h"

#include <cstdlib>

namespace Physics
{
    LinearAllocator::LinearAllocator(void* ptr, std::size_t size) :
        _ptr(ptr),
        _size(size),
        _offset(0)
    {}

    LinearAllocator::~LinearAllocator()
    {
        std::free(_ptr);
    }

    void* LinearAllocator::Allocate(std::size_t size)
    {
        if (_offset + size > _size) return nullptr;

        void* ptr = static_cast<char*>(_ptr) + _offset;
        _offset += size;

        return ptr;
    }

    void LinearAllocator::Deallocate(void* ptr)
    {
        // Do nothing
    }

    void LinearAllocator::Clear()
    {
        _offset = 0;
    }

    StackAllocator::StackAllocator(void* ptr, std::size_t size) :
        _currentPtr(ptr),
        _rootPtr(ptr),
        _topPtr(static_cast<char*>(ptr) + size),
        _prevPtr(nullptr),
        _size(size)
    {}

    StackAllocator::~StackAllocator()
    {
        std::free(_rootPtr);
    }

    void* StackAllocator::Allocate(std::size_t size)
    {
        if (static_cast<char*>(_currentPtr) + size > _topPtr) return nullptr;

        _prevPtr = _currentPtr;
        _currentPtr = static_cast<char*>(_currentPtr) + size;

        return _prevPtr;
    }

    void StackAllocator::Deallocate(void* ptr)
    {
        if (ptr == nullptr)
        {
            _currentPtr = _prevPtr;
            _prevPtr = nullptr;
        }
        else
        {
            _currentPtr = ptr;
        }
    }

    void StackAllocator::Clear()
    {
        _currentPtr = _rootPtr;
        _prevPtr = nullptr;
    }
}