#pragma once

#include "Render/RHI/Texture/TextureDesc.h"

#include <vulkan/vulkan.h>

namespace URay::Render::Vulkan
{

VkFormat ToVkFormat(Format format);
Format FromVkFormat(VkFormat format);

VkImageUsageFlags ToVkImageUsageFlags(TextureUsage usage);
VkImageAspectFlags ToVkImageAspectFlags(Format format);

} // namespace URay::Render::Vulkan
