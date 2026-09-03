#pragma once

#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RenderPass/RenderPassId.h"

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class VertexBuffer;
class IndexBuffer;
struct PipelineStateDesc;
class DescriptorSet;

struct DrawCommand
{
    RenderPassId passId = RenderPassId::Opaque;

    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;

    VertexBuffer* vertexBuffer = nullptr;
    uint32 vertexCount = 0;

    IndexBuffer* indexBuffer = nullptr;
    uint32 indexOffset = 0;
    uint32 indexCount = 0;

    PipelineStateDesc pipelineState = {};

    std::array<DescriptorSet*, 4> descriptorSets;
};

} // namespace URay::Render
