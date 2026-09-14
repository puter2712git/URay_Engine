#include "RenderSystem.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/Rendering/RenderPipeline.h"
#include "Render/Rendering/Renderer.h"
#include "Render/ResourceManager.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"

namespace URay::Render
{

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() = default;

bool RenderSystem::Initialize()
{
    Window& window = gEngine->GetWindow();
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    VulkanContextDesc desc = {};
    desc.appName = "URay Editor";
    desc.engineName = "URay Engine";

    vulkanContext = std::make_unique<VulkanContext>();
    if (!vulkanContext->Initialize(gEngine->GetWindow(), desc))
    {
        return false;
    }

    device = std::make_unique<RenderDevice>(*vulkanContext);
    if (!device->Initialize())
        return false;

    resourceManager = std::make_unique<ResourceManager>(*device, filesystem);

    renderer = std::make_unique<Renderer>(window, *vulkanContext, *device, *resourceManager);
    if (!renderer->Initialize())
        return false;

    pipeline = std::make_unique<RenderPipeline>(*this);
    if (!pipeline->Initialize())
        return false;

    return true;
}

void RenderSystem::Finalize()
{
    WaitIdle();

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
    renderer->WaitIdle();
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
