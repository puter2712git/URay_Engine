#pragma once

#include "Render/RHI/Texture/TextureDesc.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Texture
{
public:
    Texture(VkDevice device, VkImage handle, VkDeviceMemory memory, const TextureDesc& desc);
    ~Texture();

public:
    VkImage GetHandle() const { return handle; }

    const TextureDesc& GetDesc() const { return desc; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    TextureDesc desc = {};
};

} // namespace URay::Render
