#include "Texture.h"

namespace URay::Render
{

Texture::Texture(VkDevice device, VkImage handle, VkDeviceMemory memory, const TextureDesc& desc, const TextureViewDesc& viewDesc)
    : device(device), handle(handle), memory(memory), desc(desc), viewDesc(viewDesc)
{
}

Texture::~Texture()
{
    vkDestroyImage(device, handle, nullptr);
    vkFreeMemory(device, memory, nullptr);
}

} // namespace URay::Render
