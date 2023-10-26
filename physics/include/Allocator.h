#pragma once

#include <cstddef>

namespace Physics
{
    /**
     * @brief Allocator interface
     */
    class Allocator
    {
    public:
        virtual ~Allocator() = default;

        /**
         * @brief Allocate memory
         * @param size Size of memory to allocate
         * @return Pointer to allocated memory
         */
        virtual void* Allocate(size_t size) = 0;
        /**
         * @brief Deallocate memory
         * @param ptr Pointer to memory to deallocate
         */
        virtual void Deallocate(void* ptr) = 0;
    };

    /**
     * @brief Linear allocator
     */
    class LinearAllocator : public Allocator
    {
    private:
        void* _ptr;
        size_t _size;
        size_t _offset;

    public:
        /**
         * @brief Constructor
         * @param size Size of memory to allocate
         */
        LinearAllocator(void* ptr, std::size_t size);
        ~LinearAllocator() override;

        void* Allocate(std::size_t size) override;
        void Deallocate(void* ptr) override;

        /**
         * @brief Clear allocator
         */
        void Clear();
    };

    /**
     * @brief Stack allocator
     */
    class StackAllocator : public Allocator
    {
    private:
        void* _rootPtr;
        void* _topPtr;
        void* _prevPtr;
        void* _currentPtr;
        size_t _size;

    public:
        /**
         * @brief Constructor
         * @param size Size of memory to allocate
         */
        StackAllocator(void* ptr, std::size_t size);
        ~StackAllocator() override;

        void* Allocate(std::size_t size) override;
        void Deallocate(void* ptr) override;

        /**
         * @brief Clear allocator
         */
        void Clear();
    };
}