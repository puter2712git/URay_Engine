#pragma once

#include "Render/RHI/Texture/Texture.h"
#include "Render/Rendering/DrawCommand/DrawCommand.h"
#include "Render/Rendering/FrameResource.h"
#include "Render/Rendering/ObjectIdReadback.h"
#include "Render/Rendering/RenderInfo.h"

#include "Core/Math/Extent2D.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include "Core/Ray/EventRay.h"

#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace URay
{
class Window;
class VirtualFileSystem;
} // namespace URay

namespace URay::Render
{

class VulkanContext;
class Device;
class DescriptorSetLayout;
class RenderTarget;
class ResourceManager;
class Framebuffer;
class SwapChain;
class CommandPool;
class Texture;
class TextureView;
class RenderTargetRegistry;

class Renderer
{
public:
    Renderer(Window& window, VulkanContext& context, Device& device, ResourceManager& resourceManager);
    ~Renderer();

public:
    bool Initialize();
    void Finalize();

    bool InitializeImGui();
    void FinalizeImGui();

    bool BeginFrame();
    void EndFrame();

    void WaitIdle();

    void RequestSceneRenderTargetResize(const Extent2D& extent);
    Extent2D GetSceneRenderTargetExtent() const;

    VkImage GetSwapChainImage() const;
    VkImageView GetSwapChainImageView() const;
    VkExtent2D GetSwapChainExtent() const;

    VkDescriptorSet GetSceneImGuiTexture() const { return sceneImGuiTexture; }

    FrameResource& GetFrameResource() { return frameResources[currentFrame]; }

    RenderTargetRegistry& GetRenderTargetRegistry() { return *renderTargetRegistry; }

    ObjectIdReadback& GetObjectIdReadback() { return *objectIdReadback; }
    EventRay<const ObjectIdReadbackResult&>& GetObjectIdReadbackRay() { return objectIdReadbackRay; }

private:
    bool CreateSceneRenderTarget();
    bool CreatePostProcessRenderTarget();
    bool CreateSelectionMaskRenderTarget();
    bool CreateGizmoDepthRenderTarget();

    void CleanupSwapChain();
    void RecreateSwapChain();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateFrameResources();
    void DestroyFrameResources();

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
    Device& device;
    ResourceManager& resourceManager;

    std::unique_ptr<SwapChain> swapChain = nullptr;
    uint32 imageIndex = 0;

    std::unique_ptr<RenderTargetRegistry> renderTargetRegistry = nullptr;

    VkDescriptorSet sceneImGuiTexture = VK_NULL_HANDLE;

    std::unique_ptr<CommandPool> commandPool = nullptr;

    std::optional<Extent2D> pendingSceneRenderTargetExtent;

    std::unique_ptr<DescriptorSetLayout> frameDescriptorSetLayout = nullptr;
    std::array<FrameResource, MAX_FRAMES_IN_FLIGHT> frameResources;

    std::unique_ptr<ObjectIdReadback> objectIdReadback = nullptr;
    EventRay<const ObjectIdReadbackResult&> objectIdReadbackRay;
};

} // namespace URay::Render
