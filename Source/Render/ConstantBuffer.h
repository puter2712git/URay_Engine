#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class ConstantBuffer
{
public:
    ConstantBuffer(VkDevice device,
                   VkBuffer handle, VkDeviceMemory memory,
                   VkDeviceSize size);
    ~ConstantBuffer();

public:
    void UpdateData(void* data, size_t size);

    VkBuffer GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    void* mappedData = nullptr;
};

} // namespace URay
