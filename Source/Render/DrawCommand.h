#pragma once

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

namespace URay
{

class VertexBuffer;
class IndexBuffer;
class Material;

struct DrawCommand
{
    Matrix worldMatrix = Matrix::Identity;

    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    Material* material = nullptr;
};

} // namespace URay
