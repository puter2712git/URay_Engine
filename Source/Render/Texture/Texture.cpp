#include "Texture.h"

namespace URay::RHI
{

Texture::Texture(VkDevice device, VkImage handle, VkDeviceMemory memory)
    : device(device), handle(handle), memory(memory)
{
}

Texture::~Texture()
{
    if (handle)
        vkDestroyImage(device, handle, nullptr);
    if (memory)
        vkFreeMemory(device, memory, nullptr);
}

} // namespace URay::RHI
