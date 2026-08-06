#pragma once

#include "Render/PipelineState/PipelineStateDesc.h"

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

namespace URay
{

class VertexBuffer;
class IndexBuffer;
struct PipelineStateDesc;
class DescriptorSet;

struct DrawCommand
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32_t objectId = 0;

    void* vertexBuffer = nullptr;
    uint32_t vertexCount = 0;

    void* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    PipelineStateDesc pipelineState = {};

    DescriptorSet* descriptorSet = nullptr;
};

} // namespace URay
