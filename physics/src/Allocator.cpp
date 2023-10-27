#include "Allocator.h"

#include <cstdlib>

namespace Physics
{
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
		return _allocator.Allocate(size, alignment);
	}

	void ProxyAllocator::Deallocate(void* ptr) noexcept
	{
		_allocator.Deallocate(ptr);
	}

	// FreeListAllocator implementation

	FreeListAllocator::FreeListAllocator(void* ptr, std::size_t size) noexcept :
		Allocator(ptr, size), _freeBlocks(reinterpret_cast<FreeBlock*>(ptr))
	{
		_freeBlocks->size = size;
		_freeBlocks->next = nullptr;
	}

	FreeListAllocator::~FreeListAllocator()
	{
		_freeBlocks = nullptr;
	}

	void* FreeListAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
	{
		// Find the best free block
		FreeBlock* prevBlock = nullptr;
		FreeBlock* freeBlock = _freeBlocks;
		while (freeBlock != nullptr)
		{
			// Calculate padding
			std::size_t padding = 0;
			std::size_t alignedAddress = reinterpret_cast<std::size_t>(freeBlock) + sizeof(FreeBlock);
			if (alignment != 0 && alignedAddress % alignment != 0)
			{
				padding = alignment - (alignedAddress % alignment);
			}

			// Calculate total size
			std::size_t totalSize = size + padding;

			// Check if there is enough memory
			if (freeBlock->size < totalSize)
			{
				prevBlock = freeBlock;
				freeBlock = freeBlock->next;
				continue;
			}

			// Check if the block is large enough to split
			if (freeBlock->size - totalSize <= sizeof(FreeBlock))
			{
				// Remove the block from the list
				if (prevBlock != nullptr)
				{
					prevBlock->next = freeBlock->next;
				}
				else
				{
					_freeBlocks = freeBlock->next;
				}
			}
			else
			{
				// Split the block
				FreeBlock* nextBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<std::size_t>(freeBlock) + totalSize);
				nextBlock->size = freeBlock->size - totalSize;
				nextBlock->next = freeBlock->next;

				// Remove the block from the list
				if (prevBlock != nullptr)
				{
					prevBlock->next = nextBlock;
				}
				else
				{
					_freeBlocks = nextBlock;
				}
			}

			// Update the number of allocations
			_allocations++;

			return reinterpret_cast<void*>(reinterpret_cast<std::size_t>(freeBlock) + padding);
		}

		// No free block found
		return nullptr;
	}

	void FreeListAllocator::Deallocate(void* ptr) noexcept
	{
		// Add the block to the list
		auto* freeBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<std::size_t>(ptr) - sizeof(FreeBlock));
		freeBlock->next = _freeBlocks;
		_freeBlocks = freeBlock;

		// Update the number of allocations
		_allocations--;
	}
}