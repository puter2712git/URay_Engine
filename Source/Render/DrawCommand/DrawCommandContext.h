#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"

#include <cstdint>

namespace URay
{

class VertexBuffer;
class IndexBuffer;
class Material;

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32_t objectId = 0;

    VertexBuffer* vertexBuffer = nullptr;
    uint32_t vertexCount = 0;

    IndexBuffer* indexBuffer = nullptr;
    uint32_t indexCount = 0;

    Material* material = nullptr;
};

struct LineCommandContext
{
    Vector3 start = Vector3::Zero;
    Vector3 end = Vector3::Zero;
    Color color = Color::White;
};

} // namespace URay
