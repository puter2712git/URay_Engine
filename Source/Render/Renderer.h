#pragma once

#include "Render/DrawCommand/DrawCommand.h"

#include "Core/Math/Extent2D.h"
#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{
class Window;
class VirtualFilesystem;
class Scene;
} // namespace URay

namespace URay::Render
{

class VulkanContext;
class RenderDevice;
class VertexBuffer;
class IndexBuffer;
class PipelineLayout;
class DescriptorSetLayout;
class DescriptorSet;
class RenderTarget;
class GPUResourceManager;
class ShaderManager;
class RenderScene;
class Framebuffer;
class SwapChain;

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

class Renderer
{
public:
    Renderer(Window& window, VulkanContext& context, RenderDevice& device, GPUResourceManager& resourceManager);
    ~Renderer();

public:
    bool Initialize(VirtualFilesystem& filesystem);
    void Finalize();

    bool InitializeImGui(const VirtualFilesystem& filesystem);
    void FinalizeImGui();

    bool BeginFrame();
    void EndFrame();

    void BeginScenePass();
    void EndScenePass();

    void BeginSwapChainPass();
    void EndSwapChainPass();

    void BeginImGui();
    void EndImGui();

    void WaitIdle();

    void SetFrameViewInfo(const Matrix& newViewMatrix, const Matrix& newProjMatrix);

    void RequestSceneRenderTargetResize(const Extent2D& extent);
    Extent2D GetSceneRenderTargetExtent() const;

    void ClearSceneDepth(float depth, uint32_t stencil);

    void Draw(const DrawCommand& cmd);

    VkDescriptorSet GetSceneImGuiTexture() const { return sceneImGuiTexture; }

private:
    bool CreateSceneRenderPass();
    void DestroySceneRenderPass();

    bool CreateRenderPass();
    void DestroyRenderPass();

    bool CreateSceneRenderTarget();
    void DestroySceneRenderTarget();

    bool CreateSceneFramebuffer();
    void DestroySceneFramebuffer();

    void CleanupSwapChain();
    void RecreateSwapChain();

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

    void ProcessPendingSceneRenderTargetResize();

    VkFormat FindDepthFormat() const;
    bool HasStencilComponent(VkFormat format) const;

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;

public:
    bool framebufferResized = false;

private:
    Window& window;
    VulkanContext& context;
    RenderDevice& device;
    GPUResourceManager& resourceManager;

    std::unique_ptr<SwapChain> swapChain = nullptr;
    std::vector<std::unique_ptr<Framebuffer>> swapChainFramebuffers;
    uint32_t imageIndex = 0;

    std::unique_ptr<RenderTarget> sceneRenderTarget = nullptr;
    std::unique_ptr<Framebuffer> sceneFramebuffer = nullptr;
    VkDescriptorSet sceneImGuiTexture = VK_NULL_HANDLE;

    VkRenderPass sceneRenderPass = VK_NULL_HANDLE;
    VkRenderPass swapChainRenderPass = VK_NULL_HANDLE;

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

    std::optional<Extent2D> pendingSceneRenderTargetExtent;
};

} // namespace URay::Render
