#include "VulkanUtils.h"

#include <stdexcept>

namespace URay::Render::Vulkan
{

VkFormat ToVkFormat(Format format)
{
    switch (format)
    {
    case Format::RGBA8_UNorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case Format::RGBA8_sRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case Format::BGRA8_sRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case Format::D32_Float:
        return VK_FORMAT_D32_SFLOAT;
    case Format::D32_Float_S8_UInt:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case Format::D24_UNorm_S8_UInt:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

Format FromVkFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R8G8B8A8_UNORM:
        return Format::RGBA8_UNorm;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return Format::RGBA8_sRGB;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return Format::BGRA8_sRGB;
    case VK_FORMAT_D32_SFLOAT:
        return Format::D32_Float;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return Format::D32_Float_S8_UInt;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return Format::D24_UNorm_S8_UInt;
    default:
        throw std::runtime_error("Unsupported Vulkan format.");
    }
}

VkBufferUsageFlags ToVkBufferUsage(BufferBindFlags flags)
{
    VkBufferUsageFlags usage = 0;

    if ((flags & BufferBindFlags::Vertex) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    if ((flags & BufferBindFlags::Index) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    if ((flags & BufferBindFlags::Uniform) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if ((flags & BufferBindFlags::Storage) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    if ((flags & BufferBindFlags::CopySrc) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if ((flags & BufferBindFlags::CopyDst) != BufferBindFlags::None)
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    return usage;
}

VkMemoryPropertyFlags ToVkMemoryProperties(MemoryUsage usage)
{
    switch (usage)
    {
    case MemoryUsage::GpuOnly:
        return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    case MemoryUsage::CpuToGpu:
    case MemoryUsage::GpuToCpu:
        return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    return 0;
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
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    default:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

} // namespace URay::Render::Vulkan
