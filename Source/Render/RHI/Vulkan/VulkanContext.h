#include "Render/RHI/RHIContext.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace URay
{

class Window;

namespace Render
{

struct VulkanContextDesc
{
    std::string appName;
    std::string engineName;
};

class VulkanContext final : public RHIContext
{
public:
    bool Initialize(Window& window, const VulkanContextDesc& desc);
    void Finalize();

    VkInstance GetInstance() const { return instance; }
    VkSurfaceKHR GetSurface() const { return surface; }

    const std::vector<const char*>& GetValidationLayers() const { return validationLayers; }
    const std::vector<const char*>& GetDeviceExtensions() const { return deviceExtensions; }

private:
    bool CreateInstance(const VulkanContextDesc& desc);
    bool SetupDebugMessenger();
    bool CreateSurface(Window& window);

    bool CheckValidationLayerSupport() const;
    std::vector<const char*> GetRequiredExtensions() const;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

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

} // namespace Render

} // namespace URay
