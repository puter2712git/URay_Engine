#pragma once

#include "Render/RHI/Buffer/BufferUsage.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

struct BufferDesc
{
    uint64 size = 0;
    BufferBindFlags bindFlags = BufferBindFlags::None;
    MemoryUsage memoryUsage = MemoryUsage::GpuOnly;

    uint32 stride = 0;
    const void* initialData = nullptr;
    uint64 initialDataSize = 0;
};

struct VertexBufferDesc
{
    uint64 size = 0;

    uint32 vertexCount = 0;
    uint32 vertexStride = 0;
    const void* initialData = nullptr;
    uint64 initialDataSize = 0;

    MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
};

enum class IndexType
{
    UInt16,
    UInt32
};

struct IndexBufferDesc
{
    uint64 size = 0;

    uint32 indexCount = 0;
    IndexType indexType = IndexType::UInt32;
    const void* initialData = nullptr;
    uint64 initialDataSize = 0;

    MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
};

struct UniformBufferDesc
{
    uint64 size = 0;
    const void* initialData = nullptr;
    uint64 initialDataSize = 0;

    MemoryUsage memoryUsage = MemoryUsage::CpuToGpu;
};

} // namespace URay::Render
