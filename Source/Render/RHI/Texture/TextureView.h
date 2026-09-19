#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Texture;

enum class TextureViewType : uint8
{
    Texture1D,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray
};

struct TextureViewDesc
{
    TextureViewType type = TextureViewType::Texture2D;
    uint32 baseMipLevel = 0;
    uint32 mipLevelCount = 1;
    uint32 baseArrayLayer = 0;
    uint32 arrayLayerCount = 1;
};

class TextureView
{
public:
    TextureView(VkDevice device, VkImageView handle, Texture* ref, const TextureViewDesc& viewDesc);
    ~TextureView();

public:
    VkImageView GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkImageView handle = VK_NULL_HANDLE;

    Texture* refTexture = nullptr;
    TextureViewDesc desc = {};
};

} // namespace URay::Render
