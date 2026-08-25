#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Texture;

class TextureView
{
public:
    TextureView(VkDevice device, VkImageView handle, Texture* ref);
    ~TextureView();

public:
    VkImageView GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkImageView handle = VK_NULL_HANDLE;
    Texture* refTexture = nullptr;
};

} // namespace URay::Render
