#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace URay::Render::Vulkan
{

struct SurfaceSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

std::optional<SurfaceSupportDetails> QuerySurfaceSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface);

} // namespace URay::Render::Vulkan
