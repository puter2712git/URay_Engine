#pragma once

#include <memory>

namespace URay
{

class Engine;
class Window;
class VirtualFilesystem;

namespace Render
{

class VulkanContext;
class Renderer;
class RenderDevice;
class ResourceManager;
class RenderPipeline;

class RenderSystem
{
public:
    RenderSystem(Engine& engine);
    ~RenderSystem();

public:
    bool Initialize(Window& window, VirtualFilesystem& filesystem);
    void Finalize();

    bool InitializeImGui(VirtualFilesystem& filesystem);
    void FinalizeImGui();

    void WaitIdle();

    bool BeginFrame();
    void EndFrame();

    Renderer& GetRenderer() const { return *renderer; }
    RenderDevice& GetDevice() const { return *device; }
    ResourceManager& GetResourceManager() const { return *resourceManager; }

    RenderPipeline& GetPipeline() const { return *pipeline; }

private:
    Engine& engine;

    std::unique_ptr<VulkanContext> vulkanContext = nullptr;
    std::unique_ptr<Renderer> renderer = nullptr;
    std::unique_ptr<RenderDevice> device = nullptr;

    std::unique_ptr<ResourceManager> resourceManager = nullptr;

    std::unique_ptr<RenderPipeline> pipeline = nullptr;
};

} // namespace Render

} // namespace URay
