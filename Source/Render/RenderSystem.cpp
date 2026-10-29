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
#include <Render/ResourceManager.h>

namespace URay::Render
{

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

    for (auto& [window, surface] : windowSurfaces)
    {
        surface->Finalize();
    }
    windowSurfaces.clear();

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

    if (windowSurfaces.contains(&window))
        return false;

    auto surface = std::make_unique<Surface>();
    if (!surface->Initialize(vulkanContext->GetInstance(), window))
    {
        URAY_LOG("[RenderSystem] Failed to create window surface.");
        return false;
    }

    if (!device->SupportsPresent(*surface))
    {
        URAY_LOG("[RenderSystem] Present queue does not support window surface.");
        surface->Finalize();
        return false;
    }

    windowSurfaces.emplace(&window, std::move(surface));

    return true;
}

void RenderSystem::DestroyWindowSurface(Window& window)
{
    const auto it = windowSurfaces.find(&window);

    if (it == windowSurfaces.end())
        return;

    it->second->Finalize();
    windowSurfaces.erase(it);
}

} // namespace URay::Render
