#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include <cstdint>
#include <string>

namespace URay
{
class Font;
class Material;
class Mesh;
} // namespace URay

namespace URay::Render
{

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    Mesh* mesh = nullptr;
    Material* material = nullptr;

    uint32 indexOffset = 0;
    uint32 indexCount = 0;
};

struct LineCommandContext
{
    Vector3 start = Vector3::Zero;
    Vector3 end = Vector3::Zero;
    Color color = Color::White;
};

struct TextCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    std::string text;
    Font* font = nullptr;
};

struct DecalCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    AABB localBounds = {};
    Mesh* boxMesh = nullptr;
    Material* material = nullptr;
};

struct GizmoCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    ::URay::Mesh* mesh = nullptr;

    ::URay::Material* material = nullptr;
};

} // namespace URay::Render
