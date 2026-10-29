#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/RHI/RHIContext.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

struct VulkanContextDesc
{
    String appName;
    String engineName;
};

class VulkanContext final : public RHIContext
{
public:
    bool Initialize(const VulkanContextDesc& desc);
    void Finalize();

    VkInstance GetInstance() const { return instance; }

    const std::vector<const char*>& GetValidationLayers() const { return validationLayers; }
    const std::vector<const char*>& GetDeviceExtensions() const { return deviceExtensions; }

private:
    bool CreateInstance(const VulkanContextDesc& desc);
    bool SetupDebugMessenger();

    bool CheckValidationLayerSupport() const;
    bool GetRequiredExtensions(std::vector<const char*>& outExtensions) const;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

#ifdef NDEBUG
    inline static constexpr bool enableValidationLayers = false;
#else
    inline static constexpr bool enableValidationLayers = true;
#endif

    inline static const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    inline static const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
#endif
    };
};

} // namespace URay::Render
