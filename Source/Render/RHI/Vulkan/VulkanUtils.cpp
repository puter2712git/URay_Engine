#include "VulkanUtils.h"

#include <stdexcept>

namespace URay::Render::Vulkan
{

VkFormat ToVkFormat(Format format)
{
    switch (format)
    {
    case Format::RGBA8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case Format::RGBA8_sRGB: return VK_FORMAT_R8G8B8A8_SRGB;
    case Format::BGRA8_sRGB: return VK_FORMAT_B8G8R8A8_SRGB;
    case Format::D32_Float: return VK_FORMAT_D32_SFLOAT;
    case Format::D32_Float_S8_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case Format::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
    default: return VK_FORMAT_UNDEFINED;
    }
}

Format FromVkFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R8G8B8A8_UNORM: return Format::RGBA8_UNorm;
    case VK_FORMAT_R8G8B8A8_SRGB: return Format::RGBA8_sRGB;
    case VK_FORMAT_B8G8R8A8_SRGB: return Format::BGRA8_sRGB;
    case VK_FORMAT_D32_SFLOAT: return Format::D32_Float;
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32_Float_S8_UInt;
    case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24_UNorm_S8_UInt;
    default: throw std::runtime_error("Unsupported Vulkan format.");
    }
}

VkImageUsageFlags ToVkImageUsageFlags(TextureUsage usage)
{
    VkImageUsageFlags result = 0;

    if ((usage & TextureUsage::TransferSrc) != TextureUsage::None)
        result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if ((usage & TextureUsage::TransferDst) != TextureUsage::None)
        result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if ((usage & TextureUsage::Sampled) != TextureUsage::None)
        result |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if ((usage & TextureUsage::ColorAttachment) != TextureUsage::None)
        result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if ((usage & TextureUsage::DepthAttachment) != TextureUsage::None)
        result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    return result;
}

VkImageAspectFlags ToVkImageAspectFlags(Format format)
{
    switch (format)
    {
    case Format::D32_Float:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case Format::D32_Float_S8_UInt:
    case Format::D24_UNorm_S8_UInt:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

} // namespace URay::Render::Vulkan
