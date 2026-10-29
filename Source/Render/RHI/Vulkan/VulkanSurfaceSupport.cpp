#include "VulkanSurfaceSupport.h"

#include <Core/Type/TypeAlias.h>

namespace URay::Render::Vulkan
{

std::optional<SurfaceSupportDetails> QuerySurfaceSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface)
{
    SurfaceSupportDetails details;

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities) != VK_SUCCESS)
        return std::nullopt;

    uint32 formatCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr) != VK_SUCCESS)
        return std::nullopt;

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data()) != VK_SUCCESS)
            return std::nullopt;
    }

    uint32 presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr) != VK_SUCCESS)
        return std::nullopt;

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data()) != VK_SUCCESS)
            return std::nullopt;
    }

    return details;
}

} // namespace URay::Render::Vulkan
