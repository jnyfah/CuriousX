#include "arena.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

// Todo use bit manipulation for alignment

namespace cx
{

    Arena::Arena(std::size_t initSize)
    {
        std::byte* raw = static_cast<std::byte*>(::operator new(sizeof(Chunk) + initSize, std::align_val_t(alignof(std::max_align_t))));
        m_chunks       = std::construct_at(reinterpret_cast<Chunk*>(raw), initSize, alignof(std::max_align_t), nullptr);

        m_current      = m_chunks;
        m_chunkSize    = initSize;
    }

    Arena::~Arena()
    {
        release();
    }

    Arena::Arena(Arena&& other) noexcept
        : m_chunks(other.m_chunks), m_current(other.m_current), m_chunkSize(other.m_chunkSize), m_destructors(std::move(other.m_destructors))
    {
        other.m_chunks  = nullptr;
        other.m_current = nullptr;
    }

    Arena& Arena::operator=(Arena&& other) noexcept
    {
        if (this != &other)
        {
            release(); // free our current chunks first
            m_chunks        = other.m_chunks;
            m_current       = other.m_current;
            m_chunkSize     = other.m_chunkSize;
            m_destructors   = other.m_destructors;
            other.m_chunks  = nullptr;
            other.m_current = nullptr;
        }
        return *this;
    }

    void Arena::release() noexcept
    {
        // Destroy non-trivially destructible objects before releasing their arena storage.
        for (auto it = m_destructors.rbegin(); it != m_destructors.rend(); ++it)
        {
            it->destroy(it->obj);
        }
        m_destructors.clear();
        auto* current = m_chunks;
        while (current)
        {
            auto* next  = current->next;
            auto  align = current->alignment;
            std::destroy_at(current);
            ::operator delete(current, std::align_val_t(align));
            current = next;
        }
        m_chunks = m_current = nullptr;
    }

    std::byte* Arena::allocate(std::size_t size, std::size_t alignment)
    {

        auto addr    = m_current->data() + m_current->used;
        auto padding = (alignment - (std::uintptr_t(addr) % alignment)) % alignment;

        if (m_current->used + size + padding <= m_current->capacity)
        {
            m_current->used += (size + padding);
            return addr + padding;
        }

        allocateChunk(size, alignment);
        return allocate(size, alignment);
    }

    void Arena::allocateChunk(std::size_t minSize, std::size_t alignment)
    {
        alignment            = std::max(alignof(Chunk), alignment);
        const auto chunkSize = std::max(minSize + alignment, m_chunkSize);
        std::byte* raw       = static_cast<std::byte*>(::operator new(sizeof(Chunk) + chunkSize, std::align_val_t(alignment)));
        m_current->next      = std::construct_at(reinterpret_cast<Chunk*>(raw), chunkSize, alignment, nullptr);

        m_current            = m_current->next;
    }

} // namespace cx