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
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class PipelineLayout;
class DescriptorSetLayout;
class DescriptorSet;
class RenderTarget;
class GPUResourceManager;
class ShaderManager;
class RenderScene;
class Framebuffer;
class SwapChain;
class CommandPool;
class CommandBuffer;
class Texture;
class TextureView;

struct ObjectConstants
{
    Matrix world = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;
};

struct FrameConstants
{
    Matrix view = Matrix::Identity;
    Matrix invView = Matrix::Identity;
    Matrix proj = Matrix::Identity;
    Matrix invProj = Matrix::Identity;
    Matrix viewProj = Matrix::Identity;
    Matrix invViewProj = Matrix::Identity;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    Vector2 renderTargetSize = Vector2::Zero;
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

    void WaitIdle();

    void SetFrameViewInfo(const Matrix& newViewMatrix, const Matrix& newProjMatrix);

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

    bool CreateFrameConstantBuffer();
    void DestroyFrameConstantBuffer();

    bool CreateFrameDescriptorSetLayout();
    void DestroyFrameDescriptorSetLayout();

    bool CreateFrameDescriptorSet();
    void DestroyFrameDescriptorSet();

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
    GPUResourceManager& resourceManager;

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

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;

    DescriptorSetLayout* frameDescriptorSetLayout = nullptr;
    std::vector<DescriptorSet*> frameDescriptorSets;

    std::optional<Extent2D> pendingSceneRenderTargetExtent;

    std::vector<std::unique_ptr<ConstantBuffer>> frameConstantBuffers;
};

} // namespace URay::Render
