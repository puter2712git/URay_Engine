#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

enum class ImageLayout : uint8
{
    Undefined,

    ColorAttachment,
    DepthAttachment,

    ShaderReadOnly,
    DepthReadOnly,

    Present
};

enum class Format
{
    Unknown,

    RGBA8_UNorm,
    RGBA8_sRGB,

    BGRA8_sRGB,

    D32_Float,
    D32_Float_S8_UInt,
    D24_UNorm_S8_UInt,
};

enum class TextureUsage : uint32
{
    None = 0,
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    Sampled = 1 << 2,
    ColorAttachment = 1 << 3,
    DepthAttachment = 1 << 4,
};

constexpr TextureUsage operator|(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<TextureUsage>(
        static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

constexpr TextureUsage operator&(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<TextureUsage>(
        static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

enum class TextureDimension : uint8
{
    Texture1D,
    Texture2D,
    Texture3D
};

struct TextureDesc
{
    uint32 width = 0;
    uint32 height = 0;
    uint32 depth = 1;

    uint32 mipLevels = 1;
    uint32 arrayLayers = 1;

    TextureDimension dimension = TextureDimension::Texture2D;
    bool isCubeCompatible = false;

    Format format = Format::Unknown;
    TextureUsage usage = TextureUsage::None;
};

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
