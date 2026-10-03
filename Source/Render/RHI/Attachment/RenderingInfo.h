#pragma once

#include "Render/RHI/Texture/Texture.h"

#include "Core/Math/Color.h"

#include <vulkan/vulkan.h>

#include <span>

namespace URay::Render
{

enum class LoadOp : uint8
{
    Load,
    Clear,
    DontCare
};

enum class StoreOp : uint8
{
    Store,
    DontCare
};

enum class ClearValueType : uint8
{
    Float,
    UInt
};

struct ClearColorValue
{
    ClearValueType type = ClearValueType::Float;
    Color floatValue = Color::White;
    uint32 uintValue = 0;
};

struct RenderingAttachmentInfo
{
    VkImageView imageView = VK_NULL_HANDLE;
    ImageLayout layout = ImageLayout::ColorAttachment;

    LoadOp loadOp = LoadOp::Load;
    StoreOp storeOp = StoreOp::Store;

    ClearColorValue clearColor = {};
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
