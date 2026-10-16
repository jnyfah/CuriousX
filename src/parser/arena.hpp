#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

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

        //! Allocate vector in the Arena and return a view to it
        template <typename T>
        std::span<T*> copyOf(const std::vector<T*>& src)
        {
            if (src.empty())
            {
                return {};
            }
            auto* mem = reinterpret_cast<T**>(allocate(sizeof(T*) * src.size(), alignof(T*)));
            std::copy(src.begin(), src.end(), mem);
            return {mem, src.size()};
        }

        template <typename T>
        std::span<T> copyOf(const std::vector<T>& src)
        {
            if (src.empty())
            {
                return {};
            }
            auto* mem = reinterpret_cast<T*>(allocate(sizeof(T) * src.size(), alignof(T)));
            std::copy(src.begin(), src.end(), mem);
            return {mem, src.size()};
        }

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

        Chunk*      m_chunks; // head pointer
        Chunk*      m_current;
        std::size_t m_chunkSize;

        void        allocateChunk(std::size_t minSize, std::size_t alignment);
    };

} // namespace cx