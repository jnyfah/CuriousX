#include "arena.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace cx
{

    Arena::Arena(std::size_t size)
    {
        std::byte* raw = static_cast<std::byte*>(::operator new(sizeof(Chunk) + size, std::align_val_t(alignof(std::max_align_t))));
        m_chunks       = std::construct_at(reinterpret_cast<Chunk*>(raw), size, alignof(std::max_align_t), nullptr);

        m_current      = m_chunks;
        m_chunkSize    = size;
    }

    Arena::~Arena()
    {
        release();
    }

    Arena::Arena(Arena&& other) noexcept : m_chunks(other.m_chunks), m_current(other.m_current), m_chunkSize(other.m_chunkSize)
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
            other.m_chunks  = nullptr;
            other.m_current = nullptr;
        }
        return *this;
    }

    void Arena::release() noexcept
    {
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
        // How far past `base` the next correctly aligned address sits. `alignment` is
        // always a power of two, so rounding up is an add and a mask.
        std::byte* const base    = m_current->data() + m_current->used;
        const auto       addr    = reinterpret_cast<std::uintptr_t>(base);
        const auto       aligned = (addr + alignment - 1) & ~(alignment - 1);
        const auto       padding = static_cast<std::size_t>(aligned - addr);

        if (m_current->used + size + padding <= m_current->capacity)
        {
            m_current->used += (size + padding);
            return base + padding;
        }

        // allocate more chunk
        allocateChunk(size, alignment);
        return allocate(size, alignment);
    }

    void Arena::allocateChunk(std::size_t minSize, std::size_t alignment)
    {
        alignment             = std::max(alignof(Chunk), alignment);
        m_chunkSize          *= 2;
        const auto chunkSize  = std::max(minSize + alignment, m_chunkSize);
        std::byte* raw        = static_cast<std::byte*>(::operator new(sizeof(Chunk) + chunkSize, std::align_val_t(alignment)));
        m_current->next       = std::construct_at(reinterpret_cast<Chunk*>(raw), chunkSize, alignment, nullptr);

        m_current             = m_current->next;
    }

    //! Allocate vector in the Arena and return a view to it
    std::span<Node*> Arena::copyOf(const std::vector<Node*>& src)
    {
        if (src.empty())
        {
            return {};
        }
        auto* mem = reinterpret_cast<Node**>(allocate(sizeof(Node*) * src.size(), alignof(Node*)));
        std::copy(src.begin(), src.end(), mem);
        return {mem, src.size()};
    }

} // namespace cx