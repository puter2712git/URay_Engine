#pragma once

#include "Core/Type/Types.h"

namespace URay::Render
{

enum class MemoryUsage : uint8
{
    GpuOnly,
    CpuToGpu,
    GpuToCpu
};

enum class BufferBindFlags : uint32
{
    None = 0,
    Vertex = 1 << 0,
    Index = 1 << 1,
    Uniform = 1 << 2,
    Storage = 1 << 3,
    CopySrc = 1 << 4,
    CopyDst = 1 << 5
};

inline BufferBindFlags operator|(BufferBindFlags lhs, BufferBindFlags rhs)
{
    return static_cast<BufferBindFlags>(
        static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

inline BufferBindFlags operator&(BufferBindFlags lhs, BufferBindFlags rhs)
{
    return static_cast<BufferBindFlags>(
        static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

inline BufferBindFlags& operator|=(BufferBindFlags& lhs, BufferBindFlags rhs)
{
    return lhs = lhs | rhs;
}

inline BufferBindFlags& operator&=(BufferBindFlags& lhs, BufferBindFlags rhs)
{
    return lhs = lhs & rhs;
}

inline BufferBindFlags operator~(BufferBindFlags value)
{
    return static_cast<BufferBindFlags>(~static_cast<uint32>(value));
}

} // namespace URay::Render
