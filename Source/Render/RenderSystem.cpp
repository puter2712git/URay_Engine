#include "RenderSystem.h"

#include "Render/RHI/Device.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/Rendering/RenderPipeline.h"
#include "Render/Rendering/Renderer.h"
#include "Render/Rendering/Scene/SceneSystem.h"
#include "Render/ResourceManager.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"

#include "Core/Log/LogSystem.h"

namespace URay::Render
{

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() = default;

bool RenderSystem::Initialize()
{
    Window& window = gEngine->GetWindow();

    VulkanContextDesc desc = {};
    desc.appName = "URay Editor";
    desc.engineName = "URay Engine";

    vulkanContext = std::make_unique<VulkanContext>();
    if (!vulkanContext->Initialize(gEngine->GetWindow(), desc))
    {
        URAY_LOG("[RenderSystem] Failed to initialize vulkan context.");
        Finalize();
        return false;
    }

    device = std::make_unique<Device>(*vulkanContext);
    if (!device->Initialize())
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

    renderer = std::make_unique<Renderer>(window, *vulkanContext, *device, *resourceManager);
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

    if (vulkanContext)
    {
        vulkanContext->Finalize();
        vulkanContext.reset();
    }
}

bool RenderSystem::InitializeImGui()
{
    return renderer->InitializeImGui();
}

void RenderSystem::FinalizeImGui()
{
    renderer->FinalizeImGui();
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

} // namespace URay::Render
