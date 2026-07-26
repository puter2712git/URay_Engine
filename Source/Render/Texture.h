#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class Texture
{
public:
    Texture(VkDevice device, VkImage handle, VkDeviceMemory memory);
    ~Texture();

public:
    VkImage GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay
