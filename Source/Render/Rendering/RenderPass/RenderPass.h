#pragma once

#include "Render/Rendering/DrawCommand/DrawCommand.h"
#include "Render/Rendering/RenderPass/RenderPassId.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class CommandBuffer;
class ResourceManager;
class DescriptorSet;
class RenderTarget;
class Framebuffer;

class FogObject;

struct RenderPassContext
{
    CommandBuffer& commandBuffer;

    ResourceManager& resourceManager;

    DescriptorSet& frameDescriptorSet;

    RenderTarget& sceneRenderTarget;
    RenderTarget& postProcessRenderTarget;

    VkImage swapChainImage = VK_NULL_HANDLE;
    VkImageView swapChainImageView = VK_NULL_HANDLE;
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
