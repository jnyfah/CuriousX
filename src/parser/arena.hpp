#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include <span>
#include "node.hpp"

namespace cx
{
    class Arena
    {
    public:
        explicit Arena(std::size_t size);
        ~Arena();

        Arena(const Arena&)            = delete;
        Arena& operator=(const Arena&) = delete;

        Arena(Arena&& other) noexcept;
        Arena&     operator=(Arena&& other) noexcept;
        void       release() noexcept;

        std::byte* allocate(std::size_t size, std::size_t alignment);

        template <typename T, typename... Args>
        T* create(Args&&... args)
        {
            std::byte* mem = allocate(sizeof(T), alignof(T));
            T*         obj = std::construct_at(reinterpret_cast<T*>(mem), std::forward<Args>(args)...);
            return obj;
        }

        std::span<Node*> copyOf(const std::vector<Node*>& src);

    private:
        struct Chunk
        {
            Chunk(std::size_t cap, std::size_t align, Chunk* nxt) : capacity(cap), used(0), alignment(align), next(nxt) {}
            std::size_t capacity;
            std::size_t used;
            std::size_t alignment;
            Chunk*      next;

            //! chunk data is allocated after the struct, and addr is calculate when needed
            std::byte*  data()
            {
                return reinterpret_cast<std::byte*>(this + 1);
            }
        };


        Chunk*                 m_chunks; // head pointer
        Chunk*                 m_current;
        std::size_t            m_chunkSize;

        void                   allocateChunk(std::size_t minSize, std::size_t alignment);
    };

} // namespace cx