#pragma once

#include "Render/RHI/Buffer/BufferUsage.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class RenderDevice;
struct BufferDesc;

class Buffer
{
public:
    Buffer(RenderDevice& device, VkBuffer handle, VkDeviceMemory memory, const BufferDesc& desc);
    ~Buffer();

public:
    bool Update(const void* data, uint64 dataSize, uint64 offset = 0);

    VkBuffer GetHandle() const { return handle; }
    VkDeviceMemory GetMemory() const { return memory; }

    VkDeviceSize GetSize() const { return size; }
    BufferBindFlags GetBindFlags() const { return bindFlags; }
    MemoryUsage GetMemoryUsage() const { return memoryUsage; }

private:
    RenderDevice& device;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkDeviceSize size = 0;
    BufferBindFlags bindFlags = BufferBindFlags::None;
    MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
};

} // namespace URay::Render
