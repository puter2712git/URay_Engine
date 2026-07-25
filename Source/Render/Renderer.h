#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/PipelineState/PipelineState.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <unordered_map>
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

class Window;
class Scene;

class ShaderManager;
class MaterialManager;

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

public:
    bool Initialize(Window* wnd);
    void Finalize();

    bool InitializeImGui();
    void FinalizeImGui();

    void BeginFrame();
    void EndFrame();

    void BeginImGui();
    void EndImGui();

    void WaitIdle();

    void SetFrameViewInfo(const Matrix& newViewMatrix, const Matrix& newProjMatrix);

    void Draw(const DrawCommand& cmd);

    void CreatePipelineLayout();
    void DestroyPipelineLayout();

    MaterialManager* GetMaterialManager() const { return materialManager; }

    ShaderManager* GetShaderManager() const { return shaderManager; }

    RenderDevice* GetDevice() const { return renderDevice; }

    VkExtent2D GetSwapChainExtent() const { return swapChainExtent; }

    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout; }

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

    bool CreateSwapChain();
    void DestroySwapChain();

    void CleanupSwapChain();
    void RecreateSwapChain();

    bool CreateImageViews();
    void DestroyImageViews();

    bool CreateRenderPass();
    void DestroyRenderPass();

    bool CreateFramebuffers();
    void DestroyFramebuffers();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateCommandBuffer();

    bool CreateSyncObjects();
    void DestroySyncObjects();

    bool CreateDescriptorSetLayout();
    void DestroyDescriptorSetLayout();

    bool CreateUniformBuffers();
    void DestroyUniformBuffers();

    bool CreateDescriptorPool();
    void DestroyDescriptorPool();

    bool CreateDescriptorSets();

    bool CreateTextureImage();
    void DestroyTextureImage();

    bool CreateTextureImageView();
    void DestroyTextureImageView();

    bool CreateTextureSampler();
    void DestroyTextureSampler();

    bool CreateDepthResources();
    void DestroyDepthResources();

    bool CreatePersistentVertexBuffer();
    void DestroyPersistentVertexBuffer();

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

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage& image, VkDeviceMemory& imageMemory) const;
    void TransitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout) const;
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

private:
    Window* window = nullptr;

    RenderDevice* renderDevice = nullptr;

    ShaderManager* shaderManager = nullptr;
    MaterialManager* materialManager = nullptr;

    VkInstance instance = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::unordered_map<uint64_t, VkPipeline> pipelines;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t imageIndex = 0;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;

    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

public:
    VkBuffer persistentVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory persistentVertexBufferMemory = VK_NULL_HANDLE;
    void* mappedVertexBufferData = nullptr;
    uint32_t vertexCount = 0;

    void RenderLines();
};

} // namespace URay
