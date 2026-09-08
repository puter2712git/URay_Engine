#include "RenderSystem.h"

#include "Render/ResourceManager.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RenderPipeline.h"
#include "Render/Renderer.h"

#include "Engine/Engine.h"

namespace URay::Render
{

RenderSystem::RenderSystem(Engine& engine) : engine(engine) {}

RenderSystem::~RenderSystem() = default;

bool RenderSystem::Initialize(Window& window, VirtualFilesystem& filesystem)
{
    VulkanContextDesc desc = {};
    desc.appName = "URay Editor";
    desc.engineName = "URay Engine";

    vulkanContext = std::make_unique<VulkanContext>();
    if (!vulkanContext->Initialize(window, desc))
    {
        return false;
    }

    device = std::make_unique<RenderDevice>(*vulkanContext);
    if (!device->Initialize())
        return false;

    resourceManager = std::make_unique<ResourceManager>(*device, filesystem);

    renderer = std::make_unique<Renderer>(window, *vulkanContext, *device, *resourceManager);
    if (!renderer->Initialize(filesystem))
        return false;

    pipeline = std::make_unique<RenderPipeline>(engine.GetAssetSystem(), *this);
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

bool RenderSystem::InitializeImGui(VirtualFilesystem& filesystem)
{
    return renderer->InitializeImGui(filesystem);
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
