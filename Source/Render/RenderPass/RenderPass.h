#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/RenderPass/RenderPassId.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class CommandBuffer;
class GPUResourceManager;
class DescriptorSet;
class RenderTarget;
class Framebuffer;

class FogObject;

struct RenderPassContext
{
    CommandBuffer& commandBuffer;

    GPUResourceManager& resourceManager;

    DescriptorSet& frameDescriptorSet;

    RenderTarget& sceneRenderTarget;
    VkRenderPass sceneRenderPass = VK_NULL_HANDLE;
    Framebuffer& sceneFramebuffer;

    RenderTarget& postProcessRenderTarget;
    VkRenderPass postProcessRenderPass = VK_NULL_HANDLE;
    Framebuffer& postProcessFramebuffer;

    VkRenderPass swapChainRenderPass = VK_NULL_HANDLE;
    Framebuffer& swapChainFramebuffer;
    VkExtent2D swapChainExtent = {};

    FogObject* fogObject = nullptr;
};

class RenderPass
{
public:
    virtual ~RenderPass() = default;

public:
    virtual void Begin(const RenderPassContext&) = 0;
    virtual void End(const RenderPassContext&) = 0;

    virtual void Execute(const RenderPassContext&, const std::vector<DrawCommand>&) = 0;

    virtual RenderPassId GetPassId() const = 0;
};

} // namespace URay::Render
