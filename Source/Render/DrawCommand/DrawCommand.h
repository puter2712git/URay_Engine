#pragma once

#include "Render/PipelineState/PipelineState.h"

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

namespace URay
{

class VertexBuffer;
class IndexBuffer;
class PipelineState;
class Material;

struct DrawCommand
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32_t objectId = 0;

    void* vertexBuffer = nullptr;
    uint32_t vertexCount = 0;

    void* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    PipelineState pipelineState;

    Material* material = nullptr;
};

} // namespace URay
