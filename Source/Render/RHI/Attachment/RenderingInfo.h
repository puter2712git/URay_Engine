#pragma once

#include "Render/RHI/Texture/TextureDesc.h"

#include "Core/Math/Color.h"

#include <vulkan/vulkan.h>

#include <span>

namespace URay::Render
{

struct RenderingAttachmentInfo
{
    VkImageView imageView = VK_NULL_HANDLE;
    ImageLayout layout = ImageLayout::ColorAttachment;

    LoadOp loadOp = LoadOp::Load;
    StoreOp storeOp = StoreOp::Store;

    Color clearColor = Color::White;
    float clearDepth = 1.0f;
    uint32 clearStencil = 0;
};

struct RenderingInfo
{
    VkRect2D renderArea = {};
    uint32 layerCount = 1;

    std::span<const RenderingAttachmentInfo> colorAttachments;
    const RenderingAttachmentInfo* depthAttachment = nullptr;
    const RenderingAttachmentInfo* stencilAttachment = nullptr;
};

} // namespace URay::Render
