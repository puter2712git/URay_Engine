#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class RenderDevice;

class TextureCube
{
public:
private:
    RenderDevice& device;

    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay::Render
