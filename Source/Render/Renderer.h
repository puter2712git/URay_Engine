#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

#include <vulkan/vulkan.h>

#include <array>
#include <optional>
#include <vector>

namespace URay
{

struct Vertex
{
    Vector2 pos;
    Vector3 color;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {
    { { 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
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

    void DrawFrame();

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

    bool CreateVertexBuffer();
    void DestroyVertexBuffer();

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

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
};

} // namespace URay
