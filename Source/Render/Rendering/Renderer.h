#pragma once

#include <Core/Math/Extent2D.h>
#include <Core/Math/Matrix.h>
#include <Core/TimeFlow/Schedule/Scheduler.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Core/Type/TypeAlias.h>
#include <Render/RHI/Texture/Texture.h>
#include <Render/Rendering/DrawCommand/DrawCommand.h>
#include <Render/Rendering/FrameResource.h>
#include <Render/Rendering/ObjectIdReadback.h>
#include <Render/Rendering/RenderInfo.h>
#include <Render/Rendering/SceneRenderTargets.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{
class Window;
class VirtualFileSystem;
class PerformanceViewModel;
} // namespace URay

namespace URay::Render
{

class Surface;
class SurfaceRenderResource;
class Device;
class DescriptorSetLayout;
class RenderTarget;
class ResourceManager;
class Framebuffer;
class CommandPool;
class Texture;
class TextureView;
class RenderTargetRegistry;

class Renderer : public RayEmitter
{
public:
    Renderer(Window& window, Surface& surface, Device& device, ResourceManager& resourceManager);
    ~Renderer();

public:
    bool Initialize();
    void Finalize();

    bool BeginFrame();
    bool BeginFrame(SurfaceRenderResource& surfaceResource);
    void EndFrame();

    void WaitIdle();

    void RequestSceneRenderTargetResize(const Extent2D& extent);
    Extent2D GetSceneRenderTargetExtent() const;

    SceneRenderTargets GetViewportSceneRenderTargets() const;

    VkImage GetSwapChainImage() const;
    VkImageView GetSwapChainImageView() const;
    VkExtent2D GetSwapChainExtent() const;

    FrameResource& GetFrameResource() { return frameResources[currentFrame]; }

    RenderTargetRegistry& GetRenderTargetRegistry() { return *renderTargetRegistry; }

    ObjectIdReadback& GetObjectIdReadback() { return *objectIdReadback; }

    void SetPerformanceViewModel(PerformanceViewModel* viewModel) { performanceViewModel = viewModel; }

    bool IsFrameInProgress() const { return frameInProgress; }

private:
    bool BeginFrameInternal();

    bool CreateSceneRenderTarget();
    bool CreatePostProcessRenderTarget();
    bool CreateSelectionMaskRenderTarget();
    bool CreateGizmoDepthRenderTarget();

    bool CreateCommandPool();
    void DestroyCommandPool();

    bool CreateFrameResources();
    void DestroyFrameResources();

    void ProcessPendingSceneRenderTargetResize();

    bool ConsumeGPUTimestampResults(FrameResource& frameResource);

    Format FindDepthFormat() const;
    bool HasStencilComponent(VkFormat format) const;

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;

private:
    Device& device;
    ResourceManager& resourceManager;

    std::unique_ptr<SurfaceRenderResource> mainSurfaceResource;
    SurfaceRenderResource* activeSurfaceResource = nullptr;

    std::unique_ptr<RenderTargetRegistry> renderTargetRegistry = nullptr;

    std::unique_ptr<CommandPool> commandPool = nullptr;

    std::optional<Extent2D> scheduledSceneResizeExtent;
    std::optional<Extent2D> pendingSceneRenderTargetExtent;
    std::optional<Scheduler::TaskId> sceneResizeTaskId;
    static constexpr double SceneResizeDebounceSeconds = 0.15;
    std::unique_ptr<DescriptorSetLayout> frameDescriptorSetLayout = nullptr;
    std::array<FrameResource, MAX_FRAMES_IN_FLIGHT> frameResources;
    uint64 gpuTimestampSubmissionSequence = 0;
    PerformanceViewModel* performanceViewModel = nullptr;

    std::unique_ptr<ObjectIdReadback> objectIdReadback = nullptr;

    bool frameInProgress = false;
};

constexpr uint32 GpuTimestampQueryCount = 2 * static_cast<uint32>(RenderPassId::Count);

} // namespace URay::Render
