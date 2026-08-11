#pragma once

#include "Render/DrawCommand/DrawCommand.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace URay
{

struct ObjectConstants
{
    Matrix world = Matrix::Identity;
    Color colorTint = Color::White;
    uint32_t objectId = 0;
};

struct FrameConstants
{
    Matrix view = Matrix::Identity;
    Matrix proj = Matrix::Identity;
};

class RenderDevice;

class VertexBuffer;
class IndexBuffer;
class PipelineLayout;
class DescriptorSetLayout;
class DescriptorSet;

class Window;
class Scene;

class GPUResourceManager;
class ShaderManager;
class MaterialManager;

class Renderer
{
public:
    bool Initialize(Window* wnd);
    void Finalize();

    bool InitializeImGui();
    void FinalizeImGui();

    bool BeginFrame();
    void EndFrame();

    void BeginImGui();
    void EndImGui();

    void WaitIdle();

    void SetFrameViewInfo(const Matrix& newViewMatrix, const Matrix& newProjMatrix);

    void Draw(const DrawCommand& cmd);

    void CreatePipelineLayout();

    GPUResourceManager* GetResourceManager() const { return resourceManager; }

    ShaderManager* GetShaderManager() const { return shaderManager; }

    RenderDevice* GetDevice() const { return renderDevice; }

    VkExtent2D GetSwapChainExtent() const { return swapChainExtent; }

    VkRenderPass GetRenderPass() const { return renderPass; }

private:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

private:
    bool CreateInstance();
    void DestroyInstance();

    bool SetupDebugMessenger();

    bool PickPhysicalDevice();

    bool CreateLogicalDevice();
    void DestroyLogicalDevice();

    bool CreateSurface();
    void DestroySurface();

    bool CreateRenderPass();
    void DestroyRenderPass();

    bool CreateSwapChain();
    void DestroySwapChain();

    void CleanupSwapChain();
    void RecreateSwapChain();

    bool CreateSwapChainImageViews();
    void DestroySwapChainImageViews();

    bool CreateFramebuffers();
    void DestroyFramebuffers();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateCommandBuffer();

    bool CreateSyncObjects();
    void DestroySyncObjects();

    bool CreateDepthResources();
    void DestroyDepthResources();

    bool CreateFrameDescriptorSetLayout();
    void DestroyFrameDescriptorSetLayout();

    bool CreateFrameDescriptorSet();
    void DestroyFrameDescriptorSet();

    bool CheckValidationLayerSupport() const;
    std::vector<const char*> GetRequiredExtensions() const;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
    bool IsDeviceSuitable(VkPhysicalDevice device) const;
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR ChooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

    VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code) const;

    VkFormat FindDepthFormat() const;
    bool HasStencilComponent(VkFormat format) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;

public:
    bool framebufferResized = false;

private:
    Window* window = nullptr;

    RenderDevice* renderDevice = nullptr;

    GPUResourceManager* resourceManager = nullptr;

    ShaderManager* shaderManager = nullptr;

    VkInstance instance = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapChainExtent = {};
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    uint32_t imageIndex = 0;

    VkRenderPass renderPass = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;

    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

    DescriptorSetLayout* frameDescriptorSetLayout = nullptr;
    std::vector<DescriptorSet*> frameDescriptorSets;
};

} // namespace URay
