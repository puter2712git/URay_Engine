#pragma once

#include "Core/Math/Matrix.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace URay
{

struct UniformBufferObject
{
    Matrix model;
    Matrix view;
    Matrix proj;
};

const int MAX_FRAMES_IN_FLIGHT = 2;

class Window;
class Scene;

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

public:
    bool Initialize(Window* wnd);
    void Finalize();

    void Render(Scene* scene);

    void BeginFrame();
    void EndFrame();

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

    bool CreateGraphicsPipeline();
    void DestroyGraphicsPipeline();

    bool CreateRenderPass();
    void DestroyRenderPass();

    bool CreateFramebuffers();
    void DestroyFramebuffers();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateCommandBuffer();

    bool CreateSyncObjects();
    void DestroySyncObjects();

public:
    bool CreateVertexBuffer(const std::vector<Vertex>& vertices, VkBuffer& outBuffer, VkDeviceMemory& outMemory) const;
    bool CreateIndexBuffer(const std::vector<uint16_t>& indices, VkBuffer& outBuffer, VkDeviceMemory& outMemory) const;

    void DestroyBuffer(VkBuffer buffer) const;
    void FreeMemory(VkDeviceMemory memory) const;

private:
    bool CreateDescriptorSetLayout();
    void DestroyDescriptorSetLayout();

    bool CreateUniformBuffers();
    void DestroyUniformBuffers();

    bool CreateDescriptorPool();
    void DestroyDescriptorPool();

    bool CreateDescriptorSets();

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

    VkShaderModule CreateShaderModule(const std::vector<char>& code) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

private:
    Window* window = nullptr;

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
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t imageIndex = 0;
    uint32_t currentFrame = 0;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

public:
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
};

} // namespace URay
