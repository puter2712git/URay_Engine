#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

#include <algorithm>

namespace URay::Render
{

class Device;
class CommandBuffer;

enum class ImageLayout : uint8
{
    Undefined,

    TransferDst,

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

    R32_UInt,

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

struct TextureRegion
{
    uint32 x = 0;
    uint32 y = 0;
    uint32 width = 0;
    uint32 height = 0;

    bool IsEmpty() const { return width == 0 || height == 0; }

    TextureRegion Union(const TextureRegion& region) const
    {
        if (IsEmpty())
            return region;

        if (region.IsEmpty())
            return *this;

        const uint32 minX = std::min(x, region.x);
        const uint32 minY = std::min(y, region.y);

        const uint32 maxX = std::max(x + width, region.x + region.width);
        const uint32 maxY = std::max(y + height, region.y + region.height);

        TextureRegion result = {};
        result.x = minX;
        result.y = minY;
        result.width = maxX - minX;
        result.height = maxY - minY;

        return result;
    }
};

class Texture
{
public:
    Texture(Device& device, VkImage handle, VkDeviceMemory memory, const TextureDesc& desc);
    ~Texture();

private:
    struct SyncInfo
    {
        VkPipelineStageFlags2 stage;
        VkAccessFlags2 access;
    };

public:
    void Transition(CommandBuffer& commandBuffer, ImageLayout newLayout);

    VkImage GetHandle() const { return handle; }

    const TextureDesc& GetDesc() const { return desc; }

private:
    SyncInfo GetSyncInfo(ImageLayout layout);

private:
    Device& device;

    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    TextureDesc desc = {};
    ImageLayout imageLayout = ImageLayout::Undefined;
};

} // namespace URay::Render
