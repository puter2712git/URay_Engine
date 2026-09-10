#include "Buffer.h"

#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/RenderDevice.h"

namespace URay::Render
{

Buffer::Buffer(RenderDevice& device, VkBuffer handle, VkDeviceMemory memory, const BufferDesc& desc)
    : device(device), handle(handle), memory(memory),
      size(desc.size), bindFlags(desc.bindFlags), memoryUsage(desc.memoryUsage) {}

Buffer::~Buffer()
{
    if (memory)
    {
        vkFreeMemory(device.GetVKDevice(), memory, nullptr);
        memory = VK_NULL_HANDLE;
    }

    if (handle)
    {
        vkDestroyBuffer(device.GetVKDevice(), handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
}

bool Buffer::Update(const void* data, uint64 dataSize, uint64 offset)
{
    if (data == nullptr || dataSize == 0)
        return false;

    if (offset + dataSize > size)
        return false;

    return device.UpdateBuffer(*this, data, dataSize, offset);
}

void* Buffer::Map()
{
    if (memoryUsage != MemoryUsage::CpuToGpu)
        return nullptr;

    if (mappedData != nullptr)
        return mappedData;

    if (vkMapMemory(
            device.GetVKDevice(),
            memory,
            0,
            VK_WHOLE_SIZE,
            0,
            &mappedData) != VK_SUCCESS)
    {
        mappedData = nullptr;
    }

    return mappedData;
}

void Buffer::Unmap()
{
    if (!mappedData)
        return;

    vkUnmapMemory(device.GetVKDevice(), memory);
    mappedData = nullptr;
}

} // namespace URay::Render
