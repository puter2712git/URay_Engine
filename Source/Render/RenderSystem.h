#pragma once

#include <Core/Type/TypeAlias.h>
#include <memory>
#include <unordered_map>

namespace URay
{
class Engine;
class Window;
class VirtualFileSystem;
} // namespace URay

namespace URay::Render
{

class VulkanContext;
class Surface;
class Renderer;
class Device;
class ResourceManager;
class RenderPipeline;
class SceneSystem;
class SurfaceRenderResource;
struct WindowRenderContext;

class RenderSystem
{
public:
    RenderSystem();
    ~RenderSystem();

public:
    bool Initialize();
    void Finalize();

    void WaitIdle();

    bool BeginFrame();
    bool BeginFrame(Window& window);
    void EndFrame();

    bool CreateWindowSurface(Window& window);
    void DestroyWindowSurface(Window& window);

    SurfaceRenderResource* FindWindowRenderResource(Window& window) const;

    Renderer& GetRenderer() const { return *renderer; }
    Device& GetDevice() const { return *device; }
    ResourceManager& GetResourceManager() const { return *resourceManager; }

    RenderPipeline& GetPipeline() const { return *pipeline; }

    SceneSystem& GetSceneSystem() const { return *sceneSystem; }

private:
    UniquePtr<VulkanContext> vulkanContext = nullptr;
    UniquePtr<Surface> mainSurface = nullptr;
    UniquePtr<Renderer> renderer = nullptr;
    UniquePtr<Device> device = nullptr;

    UniquePtr<ResourceManager> resourceManager = nullptr;
    UniquePtr<RenderPipeline> pipeline = nullptr;
    UniquePtr<SceneSystem> sceneSystem = nullptr;

    std::unordered_map<Window*, UniquePtr<WindowRenderContext>> windowContexts;
};

} // namespace URay::Render
