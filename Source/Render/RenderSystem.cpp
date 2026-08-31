#include "RenderSystem.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RenderPipeline.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"

#include "Engine/Engine.h"

namespace URay::Render
{

RenderSystem::RenderSystem() = default;

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

    resourceManager = std::make_unique<GPUResourceManager>(device.get());

    renderer = std::make_unique<Renderer>(window, *vulkanContext, *device, *resourceManager);
    if (!renderer->Initialize(filesystem))
        return false;

    shaderManager = std::make_unique<ShaderManager>(filesystem);
    shaderManager->GetOrCreate("Sprite", "Engine://Asset/Imported/Shader/Sprite.vert.spv", "Engine://Asset/Imported/Shader/Sprite.frag.spv");
    shaderManager->GetOrCreate("Line", "Engine://Asset/Imported/Shader/Line.vert.spv", "Engine://Asset/Imported/Shader/Line.frag.spv");
    shaderManager->GetOrCreate("Mesh", "Engine://Asset/Imported/Shader/Mesh.vert.spv", "Engine://Asset/Imported/Shader/Mesh.frag.spv");
    shaderManager->GetOrCreate("Font", "Engine://Asset/Imported/Shader/Font.vert.spv", "Engine://Asset/Imported/Shader/Font.frag.spv");
    shaderManager->GetOrCreate("Fog", "Engine://Asset/Imported/Shader/Fog.vert.spv", "Engine://Asset/Imported/Shader/Fog.frag.spv");

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

    if (shaderManager)
    {
        shaderManager.reset();
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

void RenderSystem::BeginImGui()
{
    renderer->BeginImGui();
}

void RenderSystem::EndImGui()
{
    renderer->EndImGui();
}

} // namespace URay::Render
