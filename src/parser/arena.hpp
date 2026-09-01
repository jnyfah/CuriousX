#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace cx
{
    class Arena
    {
    public:
        Arena(std::size_t initSize);
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

            // for types that are not trivially destructible, keep track so it can be destroyed later 
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                m_destructors.push_back({obj, [](void* p) { static_cast<T*>(p)->~T(); }});
            }
            return obj;
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

        struct Finalizer
        {
            void* obj;
            void (*destroy)(void*);
        };

        Chunk*                 m_chunks; // head pointer
        Chunk*                 m_current;
        std::size_t            m_chunkSize;
        std::vector<Finalizer> m_destructors;

        void                   allocateChunk(std::size_t minSize, std::size_t alignment);
    };

} // namespace cx