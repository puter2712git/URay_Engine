#include "RenderSystem.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>
#include <Platform/Window/Window.h>
#include <Platform/Window/WindowSystem.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/Surface.h>
#include <Render/RHI/Vulkan/VulkanContext.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/Rendering/Renderer.h>
#include <Render/Rendering/Scene/SceneSystem.h>
#include <Render/Rendering/SurfaceRenderResource.h>
#include <Render/ResourceManager.h>

#include <stdexcept>

namespace URay::Render
{

struct WindowRenderContext
{
    std::unique_ptr<Surface> surface;
    std::unique_ptr<SurfaceRenderResource> renderResource;

    void Finalize()
    {
        if (renderResource)
        {
            renderResource->Finalize();
            renderResource.reset();
        }

        if (surface)
        {
            surface->Finalize();
            surface.reset();
        }
    }
};

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() = default;

bool RenderSystem::Initialize()
{
    Window& window = gEngine->GetWindowSystem().GetMainWindow();

    VulkanContextDesc desc = {};
    desc.appName = "URay Editor";
    desc.engineName = "URay Engine";

    vulkanContext = std::make_unique<VulkanContext>();
    if (!vulkanContext->Initialize(desc))
    {
        URAY_LOG("[RenderSystem] Failed to initialize vulkan context.");
        Finalize();
        return false;
    }

    mainSurface = std::make_unique<Surface>();
    if (!mainSurface->Initialize(vulkanContext->GetInstance(), window))
    {
        URAY_LOG("[RenderSystem] Failed to initialize main surface.");
        Finalize();
        return false;
    }

    device = std::make_unique<Device>(*vulkanContext);
    if (!device->Initialize(*mainSurface))
    {
        URAY_LOG("[RenderSystem] Failed to initialize RHI device.");
        Finalize();
        return false;
    }

    resourceManager = std::make_unique<ResourceManager>(*device);
    if (!resourceManager->Initialize())
    {
        URAY_LOG("[RenderSystem] Failed to initialize resource manager.");
        Finalize();
        return false;
    }

    renderer = std::make_unique<Renderer>(window, *mainSurface, *device, *resourceManager);
    if (!renderer->Initialize())
    {
        URAY_LOG("[RenderSystem] Failed to initialize renderer.");
        Finalize();
        return false;
    }

    pipeline = std::make_unique<RenderPipeline>(*this);
    if (!pipeline->Initialize())
    {
        URAY_LOG("[RenderSystem] Failed to initialize render pipeline.");
        Finalize();
        return false;
    }

    sceneSystem = std::make_unique<SceneSystem>();
    if (!sceneSystem->Initialize())
    {
        URAY_LOG("[RenderSystem] Failed to initialize render scene system.");
        Finalize();
        return false;
    }

    return true;
}

void RenderSystem::Finalize()
{
    WaitIdle();

    if (sceneSystem)
    {
        sceneSystem->Finalize();
        sceneSystem.reset();
    }

    if (pipeline)
    {
        pipeline->Finalize();
        pipeline.reset();
    }

    for (auto& [window, context] : windowContexts)
    {
        context->Finalize();
    }
    windowContexts.clear();

    if (renderer)
    {
        renderer->Finalize();
        renderer.reset();
    }

    if (resourceManager)
    {
        resourceManager->Finalize();
        resourceManager.reset();
    }

    if (device)
    {
        device->Finalize();
        device.reset();
    }

    if (mainSurface)
    {
        mainSurface->Finalize();
        mainSurface.reset();
    }

    if (vulkanContext)
    {
        vulkanContext->Finalize();
        vulkanContext.reset();
    }
}

void RenderSystem::WaitIdle()
{
    if (renderer)
    {
        renderer->WaitIdle();
    }
}

bool RenderSystem::BeginFrame()
{
    return renderer->BeginFrame();
}

bool RenderSystem::BeginFrame(Window& window)
{
    if (&window == &gEngine->GetWindowSystem().GetMainWindow())
        return renderer->BeginFrame();

    SurfaceRenderResource* resource = FindWindowRenderResource(window);
    if (!resource)
        return false;

    return renderer->BeginFrame(*resource);
}

void RenderSystem::EndFrame()
{
    renderer->EndFrame();
}

bool RenderSystem::CreateWindowSurface(Window& window)
{
    if (!vulkanContext || !device)
        return false;

    if (&window == &gEngine->GetWindowSystem().GetMainWindow())
        return false;

    if (windowContexts.contains(&window))
        return false;

    auto context = std::make_unique<WindowRenderContext>();
    context->surface = std::make_unique<Surface>();

    if (!context->surface->Initialize(vulkanContext->GetInstance(), window))
    {
        URAY_LOG("[RenderSystem] Failed to create window surface.");
        context->Finalize();
        return false;
    }

    if (!device->SupportsPresent(*context->surface))
    {
        URAY_LOG("[RenderSystem] Present queue does not support window surface.");
        context->Finalize();
        return false;
    }

    context->renderResource = std::make_unique<SurfaceRenderResource>(window, *context->surface, *device);

    if (!context->renderResource->Initialize())
    {
        URAY_LOG("[RenderSystem] Failed to initialize window render resource.");
        context->Finalize();
        return false;
    }

    if (context->renderResource->GetFormat() != VK_FORMAT_B8G8R8A8_SRGB)
    {
        URAY_LOG("[RenderSystem] Window swapchain format is not supported by UIPass.");
        context->Finalize();
        return false;
    }

    windowContexts.emplace(&window, std::move(context));

    return true;
}

void RenderSystem::DestroyWindowSurface(Window& window)
{
    const auto it = windowContexts.find(&window);
    if (it == windowContexts.end())
        return;

    if (renderer && renderer->IsFrameInProgress())
        throw std::runtime_error("Cannot destroy window render resources during a frame.");

    WaitIdle();

    it->second->Finalize();
    windowContexts.erase(it);
}

SurfaceRenderResource* RenderSystem::FindWindowRenderResource(Window& window) const
{
    const auto it = windowContexts.find(&window);
    if (it == windowContexts.end())
        return nullptr;

    return it->second->renderResource.get();
}

} // namespace URay::Render
