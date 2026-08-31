#pragma once

#include <cstddef>
#include <memory>
#include <utility>

namespace cx
{
    class Arena
    {
    public:
        Arena(std::size_t initSize);
        ~Arena();

        std::byte* allocate(std::size_t size, std::size_t alignment);

        template <typename T, typename... Args>
        T* create(Args&&... args)
        {
            std::byte* mem = allocate(sizeof(T), alignof(T));
            return std::construct_at(reinterpret_cast<T*>(mem), std::forward<Args>(args)...);
        }

    private:
        struct Chunk
        {
            Chunk(std::size_t cap, std::size_t align, Chunk* nxt) : capacity(cap), used(0), alignment(align), next(nxt) {}
            std::size_t capacity;
            std::size_t used;
            std::size_t alignment;
            Chunk*      next;

            // chunk data is allocated after the struct, and addr is calculate when needed
            std::byte*  data()
            {
                return reinterpret_cast<std::byte*>(this + 1);
            }
        };

        Chunk*      m_current;
        Chunk*      m_chunks; // head pointer
        std::size_t m_chunkSize;

        void        allocateChunk(std::size_t minSize, std::size_t alignment);
    };

} // namespace cx