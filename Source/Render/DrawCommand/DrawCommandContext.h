#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"

#include <cstdint>
#include <string>

namespace URay
{

class MeshAsset;
class Material;

class FontAsset;

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    MeshAsset* meshAsset = nullptr;

    Material* material = nullptr;
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
    FontAsset* font = nullptr;
};

struct GizmoCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    MeshAsset* meshAsset = nullptr;

    Material* material = nullptr;
};

} // namespace URay
