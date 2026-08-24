#include "Texture.h"

namespace URay::Render
{

Texture::Texture(VkDevice device, VkImage handle, VkDeviceMemory memory, const TextureDesc& desc)
    : device(device), handle(handle), memory(memory), desc(desc)
{
}

Texture::~Texture()
{
    if (handle)
        vkDestroyImage(device, handle, nullptr);
    if (memory)
        vkFreeMemory(device, memory, nullptr);
}

} // namespace URay::Render
