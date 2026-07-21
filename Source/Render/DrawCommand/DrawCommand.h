#pragma once

#include "Render/PipelineState/PipelineState.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

namespace URay
{

class VertexBuffer;
class IndexBuffer;
class PipelineState;

struct DrawCommand
{
    Matrix worldMatrix = Matrix::Identity;

    void* vertexBuffer = nullptr;
    uint32_t vertexCount = 0;

    void* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    PipelineState pipelineState;
};

} // namespace URay
