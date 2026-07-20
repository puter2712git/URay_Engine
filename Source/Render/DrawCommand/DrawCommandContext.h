#pragma once

#include "Core/Math/Matrix.h"

namespace URay
{

class VertexBuffer;
class IndexBuffer;
class Material;

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;

    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    Material* material = nullptr;
};

struct LineCommandContext
{
    Vector3 start;
    Vector3 end;
};

} // namespace URay
