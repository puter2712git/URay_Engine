#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/RHI/Texture/TextureDesc.h"

#include "Core/Math/Extent2D.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

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
class Buffer;
class VertexBuffer;
class IndexBuffer;
class PipelineLayout;
class DescriptorSetLayout;
class DescriptorSet;
class RenderTarget;
class ResourceManager;
class ShaderManager;
class RenderScene;
class Framebuffer;
class SwapChain;
class CommandPool;
class CommandBuffer;
class Texture;
class TextureView;

class Renderer
{
public:
    Renderer(Window& window, VulkanContext& context, RenderDevice& device, ResourceManager& resourceManager);
    ~Renderer();

public:
    bool Initialize(VirtualFilesystem& filesystem);
    void Finalize();

    bool InitializeImGui(const VirtualFilesystem& filesystem);
    void FinalizeImGui();

    bool BeginFrame();
    void EndFrame();

    void WaitIdle();

    void RequestSceneRenderTargetResize(const Extent2D& extent);
    Extent2D GetSceneRenderTargetExtent() const;

    CommandBuffer& GetCommandBuffer() const;
    DescriptorSet& GetFrameDescriptorSet() const;

    RenderTarget& GetSceneRenderTarget() const { return *sceneRenderTarget; }
    VkRenderPass GetSceneRenderPass() const { return sceneRenderPass; }
    Framebuffer& GetSceneFramebuffer() const { return *sceneFramebuffer; }

    RenderTarget& GetPostProcessRenderTarget() const { return *postProcessRenderTarget; }
    VkRenderPass GetPostProcessRenderPass() const { return postProcessRenderPass; }
    Framebuffer& GetPostProcessFramebuffer() const { return *postProcessFramebuffer; }

    VkRenderPass GetSwapChainRenderPass() const { return swapChainRenderPass; }
    Framebuffer& GetSwapChainFramebuffer() const;

    VkExtent2D GetSwapChainExtent() const;

    VkDescriptorSet GetSceneImGuiTexture() const { return sceneImGuiTexture; }

    Buffer* GetFrameUniformBuffer(uint32 frameIndex) const { return frameUniformBuffers[frameIndex].get(); }
    Buffer* GetPointLightStorageBuffer(uint32 frameIndex) const { return pointLightStorageBuffers[frameIndex].get(); }

private:
    bool CreateSceneRenderPass();
    void DestroySceneRenderPass();

    bool CreatePostProcessRenderPass();
    void DestroyPostProcessRenderPass();

    bool CreateRenderPass();
    void DestroyRenderPass();

    bool CreateSceneRenderTarget();
    void DestroySceneRenderTarget();

    bool CreatePostProcessRenderTarget();
    void DestroyPostProcessRenderTarget();

    bool CreateSceneFramebuffer();
    void DestroySceneFramebuffer();

    bool CreatePostProcessFramebuffer();
    void DestroyPostProcessFramebuffer();

    bool CreateSwapChainFramebuffer();
    void DestroySwapChainFramebuffer();

    void CleanupSwapChain();
    void RecreateSwapChain();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateCommandBuffer();

    bool CreateSyncObjects();
    void DestroySyncObjects();

    bool CreateDepthResources();
    void DestroyDepthResources();

    bool CreateFrameUniformBuffer();
    void DestroyFrameUniformBuffer();

    bool CreateFrameDescriptorSetLayout();
    void DestroyFrameDescriptorSetLayout();

    bool CreateFrameDescriptorSet();
    void DestroyFrameDescriptorSet();

    bool CreatePointLightStorageBuffer();
    void DestroyPointLightStorageBuffer();

    void ProcessPendingSceneRenderTargetResize();

    Format FindDepthFormat() const;
    bool HasStencilComponent(VkFormat format) const;

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;

public:
    bool framebufferResized = false;

private:
    Window& window;
    VulkanContext& context;
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::unique_ptr<SwapChain> swapChain = nullptr;
    std::vector<std::unique_ptr<Framebuffer>> swapChainFramebuffers;
    uint32 imageIndex = 0;

    std::unique_ptr<RenderTarget> sceneRenderTarget = nullptr;
    std::unique_ptr<Framebuffer> sceneFramebuffer = nullptr;

    std::unique_ptr<RenderTarget> postProcessRenderTarget = nullptr;
    std::unique_ptr<Framebuffer> postProcessFramebuffer = nullptr;

    VkDescriptorSet sceneImGuiTexture = VK_NULL_HANDLE;

    VkRenderPass sceneRenderPass = VK_NULL_HANDLE;
    VkRenderPass postProcessRenderPass = VK_NULL_HANDLE;
    VkRenderPass swapChainRenderPass = VK_NULL_HANDLE;

    std::unique_ptr<CommandPool> commandPool = nullptr;
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;

    DescriptorSetLayout* frameDescriptorSetLayout = nullptr;
    std::vector<DescriptorSet*> frameDescriptorSets;

    std::optional<Extent2D> pendingSceneRenderTargetExtent;

    std::vector<std::unique_ptr<Buffer>> frameUniformBuffers;
    std::vector<std::unique_ptr<Buffer>> pointLightStorageBuffers;
};

} // namespace URay::Render
