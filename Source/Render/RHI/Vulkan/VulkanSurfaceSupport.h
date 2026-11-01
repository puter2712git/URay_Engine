#pragma once

#include <Core/Type/TypeAlias.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace URay::Render::Vulkan
{

struct SurfaceSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    Vector<VkSurfaceFormatKHR> formats;
    Vector<VkPresentModeKHR> presentModes;
};

std::optional<SurfaceSupportDetails> QuerySurfaceSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface);

} // namespace URay::Render::Vulkan
