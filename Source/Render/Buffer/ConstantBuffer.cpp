#include "ConstantBuffer.h"

#include <cstring>

namespace URay::Render
{

ConstantBuffer::ConstantBuffer(VkDevice device,
                               VkBuffer handle, VkDeviceMemory memory,
                               VkDeviceSize size)
    : device(device), handle(handle), memory(memory)
{
    vkMapMemory(device, memory, 0, size, 0, &mappedData);
}

ConstantBuffer::~ConstantBuffer()
{
    if (handle)
        vkDestroyBuffer(device, handle, nullptr);
    if (memory)
        vkFreeMemory(device, memory, nullptr);
}

void ConstantBuffer::UpdateData(void* data, size_t size)
{
    std::memcpy(mappedData, data, size);
}

} // namespace URay::Render
